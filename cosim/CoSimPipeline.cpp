#include "cosim/CoSimPipeline.hpp"
#include "simdb/pipeline/Pipeline.hpp"
#include "simdb/pipeline/elements/Buffer.hpp"
#include "simdb/pipeline/elements/CircularBuffer.hpp"
#include "simdb/pipeline/elements/Function.hpp"
#include "simdb/pipeline/AsyncDatabaseAccessor.hpp"
#include "simdb/utils/Compress.hpp"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>

namespace pegasus::cosim
{

    ////////////////////////////////////////////////////////////////////////////////////////
    void CoSimPipeline::defineSchema(simdb::Schema & schema)
    {
        using dt = simdb::SqlDataType;

        auto & tbl = schema.addTable("CompressedEvents");
        tbl.addColumn("StartEuid", dt::int64_t);
        tbl.addColumn("EndEuid", dt::int64_t);
        tbl.addColumn("ZlibBlob", dt::blob_t);
        tbl.disableAutoIncPrimaryKey();
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    void CoSimPipeline::setNumHarts(size_t num_harts) { hart_pipelines_.reserve(num_harts); }

    ////////////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<simdb::pipeline::Pipeline>
    CoSimPipeline::createPipeline(simdb::pipeline::AsyncDatabaseAccessor* db_accessor)
    {
        assert(hart_pipelines_.capacity() > 0);

        auto pipeline = std::make_unique<simdb::pipeline::Pipeline>(db_mgr_, NAME);

        for (HartId hart_id = 0; hart_id < hart_pipelines_.capacity(); ++hart_id)
        {
            hart_pipelines_.emplace_back(std::make_unique<CoSimHartPipeline>(db_accessor, hart_id));
            auto hart_pipeline = hart_pipelines_.at(hart_id).get();

            pipeline_input_queues_.emplace_back(std::make_unique<simdb::ConcurrentQueue<Event>>());
            auto pipeline_input_queue = pipeline_input_queues_.back().get();

            sim_stopped_.emplace_back(false);

            // Asynchronously read the oldest event from our std::deque and send it down the
            // pipeline.
            using EventProducerFunction = simdb::pipeline::Function<void, Event>;

            auto async_source = simdb::pipeline::createTask<EventProducerFunction>(
                [pipeline_input_queue,
                 send_evt = Event()](simdb::ConcurrentQueue<Event> & out) mutable -> bool
                {
                    if (pipeline_input_queue->try_pop(send_evt))
                    {
                        out.emplace(std::move(send_evt));
                        return true;
                    }
                    return false;
                });

            // Run the events through a 100-to-1 buffer
            auto source_buffer = simdb::pipeline::createTask<simdb::pipeline::Buffer<Event>>(100);

            // Pre-validated "range" of events (euid auto-inc by 1 in the event vector)
            using ConvertToRangeFunction = simdb::pipeline::Function<EventBuffer, EventsRange>;

            auto convert_to_range = simdb::pipeline::createTask<ConvertToRangeFunction>(
                [](EventBuffer && evts, simdb::ConcurrentQueue<EventsRange> & out)
                {
                    auto euid = evts.front().getEuid();
                    for (size_t i = 1; i < evts.size(); ++i)
                    {
                        if (evts[i].getEuid() != euid + 1)
                        {
                            throw simdb::DBException("Could not validate event UIDs");
                        }
                        ++euid;
                    }

                    EventsRange range;
                    range.euid_range =
                        std::make_pair(evts.front().getEuid(), evts.back().getEuid());
                    range.events = std::move(evts);
                    out.emplace(std::move(range));
                });

            // Perform boost::serialization on the range of events (turn them into char buffers)
            using BoostSerializerFunction =
                simdb::pipeline::Function<EventsRange, EventsRangeAsBytes>;

            auto to_bytes = simdb::pipeline::createTask<BoostSerializerFunction>(
                [](EventsRange && evts, simdb::ConcurrentQueue<EventsRangeAsBytes> & out)
                {
                    EventsRangeAsBytes range_as_bytes;
                    range_as_bytes.euid_range = evts.euid_range;
                    std::vector<char> & buffer = range_as_bytes.event_bytes;

                    boost::iostreams::back_insert_device<std::vector<char>> inserter(buffer);
                    boost::iostreams::stream<
                        boost::iostreams::back_insert_device<std::vector<char>>>
                        os(inserter);
                    boost::archive::binary_oarchive oa(os);
                    oa << evts;
                    os.flush();

                    out.emplace(std::move(range_as_bytes));
                });

            // Perform zlib compression on the event ranges
            using ZlibFunction = simdb::pipeline::Function<EventsRangeAsBytes, EventsRangeAsBytes>;

            auto zlib = simdb::pipeline::createTask<ZlibFunction>(
                [](EventsRangeAsBytes && uncompressed,
                   simdb::ConcurrentQueue<EventsRangeAsBytes> & out)
                {
                    EventsRangeAsBytes compressed;
                    compressed.euid_range = uncompressed.euid_range;
                    simdb::compressData(uncompressed.event_bytes, compressed.event_bytes);
                    out.emplace(std::move(compressed));
                });

            // Write the compressed event ranges to disk
            auto async_writer =
                db_accessor->createAsyncWriter<CoSimPipeline, EventsRangeAsBytes, EuidRange>(
                    [](EventsRangeAsBytes && evts, simdb::ConcurrentQueue<EuidRange> & out,
                       simdb::pipeline::AppPreparedINSERTs* tables)
                    {
                        auto inserter = tables->getPreparedINSERT("CompressedEvents");
                        inserter->setColumnValue(0, evts.euid_range.first);
                        inserter->setColumnValue(1, evts.euid_range.second);
                        inserter->setColumnValue(2, evts.event_bytes);
                        inserter->createRecord();

                        // Send this euid range for eviction from the cache
                        out.emplace(std::move(evts.euid_range));
                    });

            // Put a circular buffer between the async writer and the eviction task.
            // We'll do this to yield a larger event cache of around 1000 events.
            // (Lag evictions by 10 euid ranges with each range spanning 100 events).
            using EvictionRing = simdb::CircularBuffer<EuidRange, 10>;
            auto eviction_ring = simdb::pipeline::createTask<EvictionRing>();

            // This task receives eviction notices from the sqlite task and updates the cache.
            // Note the different std::function signature is due to this being a terminal function,
            // i.e. <T,void> (different compared to most of the other Functions above)
            using EvictionFunction = simdb::pipeline::Function<EuidRange, void>;

            auto eviction_task = simdb::pipeline::createTask<EvictionFunction>(
                [hart_pipeline](EuidRange && eviction_euid_range) mutable
                { hart_pipeline->evict(eviction_euid_range); });

            // Connect tasks
            // ---------------------------------------------------------------------------
            *async_source >> *source_buffer >> *convert_to_range >> *to_bytes >> *zlib
                >> *async_writer >> *eviction_ring >> *eviction_task;

            // Assign threads (task groups)
            // ------------------------------------------------------------
            auto task_group =
                pipeline->createTaskGroup("DataProcessing (hart " + std::to_string(hart_id) + ")");

            // Tasks to send data to the database thread:
            task_group->addTask(std::move(async_source));
            task_group->addTask(std::move(source_buffer));
            task_group->addTask(std::move(convert_to_range));
            task_group->addTask(std::move(to_bytes));
            task_group->addTask(std::move(zlib));

            // Tasks to process eviction notices from the database thread:
            task_group->addTask(std::move(eviction_ring));
            task_group->addTask(std::move(eviction_task));

            // Note the async writer was not added to a task group. SimDB enforces
            // that database writes/reads all occur on the same shared thread, so
            // it already went ahead and added async_writer to the appropriate group
            // under the hood for us.
        }

        return pipeline;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    void CoSimPipeline::setSnooper(CoSimPipelineSnooper* snooper) { snooper_ = snooper; }

    ////////////////////////////////////////////////////////////////////////////////////////
    void CoSimPipeline::process(cosim::Event && evt)
    {
        auto hart_id = evt.getHartId();

        if (simStopped(hart_id))
        {
            throw simdb::DBException("Cannot add events to pipeline - sim has stopped");
        }

        hart_pipelines_.at(hart_id)->addToCache(evt);
        pipeline_input_queues_.at(hart_id)->emplace(std::move(evt));

        if (snooper_)
        {
            snooper_->onNewEvent(getLastEvent(hart_id));
        }
    }

    void CoSimPipeline::CoSimHartPipeline::addToCache(const Event & evt)
    {
        Event evt_copy = evt;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            last_event_uid_ = evt_copy.getEuid();
            evt_cache_.emplace_back(std::move(evt_copy));
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    uint64_t CoSimPipeline::getLastEventUID(HartId hart_id) const
    {
        return hart_pipelines_.at(hart_id)->getLastEventUID();
    }

    uint64_t CoSimPipeline::CoSimHartPipeline::getLastEventUID() const { return last_event_uid_; }

    EventAccessor CoSimPipeline::getLastEvent(HartId hart_id)
    {
        return EventAccessor(getLastEventUID(hart_id), hart_id, this);
    }

    EventAccessor CoSimPipeline::getEvent(uint64_t euid, HartId hart_id)
    {
        return EventAccessor(euid, hart_id, this);
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    void CoSimPipeline::stopSim(HartId hart_id)
    {
        StopEvent stop_event;
        hart_pipelines_.at(hart_id)->stopSim(stop_event);
        sim_stopped_.at(hart_id) = true;
    }

    bool CoSimPipeline::simStopped(HartId hart_id) const { return sim_stopped_.at(hart_id); }

    ////////////////////////////////////////////////////////////////////////////////////////
    void CoSimPipeline::postTeardown()
    {
        for (auto & pipeline : hart_pipelines_)
        {
            pipeline->postTeardown();
        }
    }

    void CoSimPipeline::CoSimHartPipeline::postTeardown()
    {
        std::cout << "Event accesses for hart " << hart_id_ << ":\n";
        std::cout << "    From cache: " << num_evts_retrieved_from_cache_ << "\n";

        if (avg_microseconds_recreating_evts_.count())
        {
            auto avg_latency_us = avg_microseconds_recreating_evts_.mean();
            std::cout << "    From disk:  " << avg_microseconds_recreating_evts_.count();
            std::cout << " (avg latency " << size_t(avg_latency_us) << " microseconds)\n\n";
        }
        else
        {
            std::cout << "    From disk:  0\n\n";
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    const Event* EventAccessor::operator->()
    {
        return get();
    }

    const Event* EventAccessor::get()
    {
        if (recreated_evt_)
        {
            return recreated_evt_.get();
        }

        if (auto evt = cosim_pipeline_->getEventFromCache_(euid_, hart_id_))
        {
            return evt;
        }

        if (auto evt = cosim_pipeline_->recreateEventFromDisk_(euid_, hart_id_))
        {
            recreated_evt_ = std::move(evt);
            return recreated_evt_.get();
        }

        throw simdb::DBException("Unable to find the event with euid ") << euid_;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    const Event* CoSimPipeline::getEventFromCache_(uint64_t euid, HartId hart_id) const
    {
        return hart_pipelines_.at(hart_id)->getEventFromCache(euid);
    }

    const Event* CoSimPipeline::CoSimHartPipeline::getEventFromCache(uint64_t euid) const
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!evt_cache_.empty() && euid >= evt_cache_.front().getEuid())
        {
            auto index = euid - evt_cache_.front().getEuid();
            if (index < evt_cache_.size())
            {
                ++num_evts_retrieved_from_cache_;
                return &evt_cache_[index];
            }
        }

        return nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<Event> CoSimPipeline::recreateEventFromDisk_(uint64_t euid, HartId hart_id)
    {
        return hart_pipelines_.at(hart_id)->recreateEventFromDisk(euid);
    }

    std::unique_ptr<Event> CoSimPipeline::CoSimHartPipeline::recreateEventFromDisk(uint64_t euid)
    {
        EventsRangeAsBytes compressed_evts_range;

        auto query_func = [&](simdb::DatabaseManager* db_mgr)
        {
            auto query = db_mgr->createQuery("CompressedEvents");
            query->addConstraintForInt("StartEuid", simdb::Constraints::LESS_EQUAL, euid);
            query->addConstraintForInt("EndEuid", simdb::Constraints::GREATER_EQUAL, euid);

            int64_t start, end;
            query->select("StartEuid", start);
            query->select("EndEuid", end);
            query->select("ZlibBlob", compressed_evts_range.event_bytes);

            auto result_set = query->getResultSet();
            if (result_set.getNextRecord())
            {
                compressed_evts_range.euid_range = std::make_pair(start, end);
            }
        };

        // Keep track of how long we spend waiting for a response
        auto start = std::chrono::high_resolution_clock::now();

        // Run the query on the database thread. It will stop what it is doing
        // as quickly as possible to run this query. The eval() method blocks
        // until the query is picked up and run.
        async_eval_->eval(query_func);
        if (compressed_evts_range.event_bytes.empty())
        {
            return nullptr;
        }

        // "Undo" the pipeline transforms. Start by undoing zlib.
        EventsRangeAsBytes evts_range_as_bytes;
        simdb::decompressData(compressed_evts_range.event_bytes, evts_range_as_bytes.event_bytes);

        // Now use boost::serialization to turn the char buffer into an EventsRange.
        namespace bio = boost::iostreams;
        auto & bytes = evts_range_as_bytes.event_bytes;
        bio::array_source src(bytes.data(), bytes.size());
        bio::stream<bio::array_source> is(src);

        boost::archive::binary_iarchive ia(is);
        EventsRange evts_range;
        ia >> evts_range;

        // If we got this far, the event uid must be within the range.
        if (euid < evts_range.euid_range.first)
        {
            throw simdb::DBException("Internal error occurred. Cannot find event with uid ")
                << euid << ".";
        }

        auto index = euid - evts_range.euid_range.first;
        if (index >= evts_range.events.size())
        {
            throw simdb::DBException("Internal error occurred. Cannot find event with uid ")
                << euid << ".";
        }

        // Keep track of how long we spend waiting for a response
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dur = end - start;
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

        // Add to the simdb::utils::RunningMean
        avg_microseconds_recreating_evts_.add(us);

        // Return the requested event.
        auto evt = std::move(evts_range.events[index]);
        return std::make_unique<Event>(std::move(evt));
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    void CoSimPipeline::CoSimHartPipeline::evict(const EuidRange & euid_range)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        // Verify we are always evicting from the back of the cache.
        if (evt_cache_.front().getEuid() != euid_range.first)
        {
            throw simdb::DBException("Invalid event uid range");
        }

        auto num_evictions = euid_range.second - euid_range.first + 1;
        if (num_evictions > evt_cache_.size())
        {
            throw simdb::DBException("Invalid event uid range");
        }

        auto first = evt_cache_.cbegin();
        auto last = first + num_evictions;
        evt_cache_.erase(first, last);
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    void CoSimPipeline::CoSimHartPipeline::stopSim(StopEvent & stop_event)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!evt_cache_.empty())
        {
            auto & evt = evt_cache_.front();
            stop_event.stopSim(evt);
        }
    }

} // namespace pegasus::cosim

#include "simdb/apps/AppRegistration.hpp"
REGISTER_SIMDB_APPLICATION(pegasus::cosim::CoSimPipeline);
