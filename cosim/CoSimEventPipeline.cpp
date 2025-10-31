#include "cosim/CoSimEventPipeline.hpp"
#include "core/observers/CoSimObserver.hpp"
#include "core/PegasusState.hpp"
#include "core/Execute.hpp"
#include "simdb/pipeline/PipelineManager.hpp"
#include "simdb/pipeline/elements/Function.hpp"
#include "simdb/pipeline/AsyncDatabaseAccessor.hpp"
#include "simdb/utils/Compress.hpp"
#include "sparta/serialization/checkpoint/DatabaseCheckpointer.hpp"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>

namespace pegasus::cosim
{

    CoSimEventPipeline::CoSimEventPipeline(simdb::DatabaseManager* db_mgr, CoreId core_id, HartId hart_id, PegasusState* state) :
        db_mgr_(db_mgr),
        core_id_(core_id),
        hart_id_(hart_id),
        state_(state)
    {
        auto & ext_mgr = state->getExtensionManager();
        const auto & ext_map = ext_mgr.getEnabledExtensions();
        for (const auto & [ext_name, _] : ext_map)
        {
            if (ext_map.isEnabled(ext_name))
            {
                uncommitted_evts_enabled_extensions_baseline_.insert(ext_name);
            }
        }

        ext_mgr.registerExtensionChangeCallback(
            [this](const std::string & extension_name, bool enabled)
            {
                if (flushing_)
                {
                    return;
                }

                sparta_assert(observer_ != nullptr, "CoSimObserver never set!");
                Event & last_event = observer_->last_event_;
                if (enabled)
                {
                    last_event.enabled_extensions_.insert(extension_name);
                }
                else
                {
                    last_event.disabled_extensions_.insert(extension_name);
                }
            });
    }

    void CoSimEventPipeline::setObserver(CoSimObserver* observer)
    {
        sparta_assert(observer_ == nullptr || observer_ == observer,
                      "CoSimEventPipeline observer can only be set once!");
        observer_ = observer;
    }

    void CoSimEventPipeline::defineSchema(simdb::Schema & schema)
    {
        using dt = simdb::SqlDataType;

        auto & tbl = schema.addTable("CompressedEvents");

        // Contiguous event UID range covered by this blob
        tbl.addColumn("StartEuid", dt::uint64_t);
        tbl.addColumn("EndEuid", dt::uint64_t);

        // Remember that all cores/harts share the same database,
        // so we need to distinguish events by core/hart ID.
        tbl.addColumn("CoreId", dt::int32_t);
        tbl.addColumn("HartId", dt::int32_t);

        // The compressed event data blob
        tbl.addColumn("ZlibBlob", dt::blob_t);

        // Index for fast lookup by euid range and core/hart ID.
        tbl.createCompoundIndexOn({"StartEuid", "EndEuid", "CoreId", "HartId"});
        tbl.disableAutoIncPrimaryKey();
    }

    void CoSimEventPipeline::createPipeline(simdb::pipeline::PipelineManager* pipeline_mgr)
    {
        auto db_accessor = pipeline_mgr->getAsyncDatabaseAccessor();
        auto pipeline = pipeline_mgr->createPipeline(NAME);

        pipeline_mgr_ = pipeline_mgr;
        async_eval_ = db_accessor;

        // Task 1: Run the incoming events through boost::serialization
        using EventList = std::deque<Event>;

        auto serialize =
            simdb::pipeline::createTask<simdb::pipeline::Function<EventList, SerializedEvtsBuffer>>(
                [this](EventList && evts, simdb::ConcurrentQueue<SerializedEvtsBuffer> & out,
                       bool /*force_flush*/)
                {
                    // Validate all core/hart IDs match our expected IDs
                    for (const auto & evt : evts)
                    {
                        sparta_assert(evt.getCoreId() == core_id_);
                        sparta_assert(evt.getHartId() == hart_id_);
                    }

                    // Validate that all event UIDs are contiguous
                    for (size_t i = 1; i < evts.size(); ++i)
                    {
                        if (evts[i].getEuid() != evts[i - 1].getEuid() + 1)
                        {
                            throw simdb::DBException("Could not validate event IDs");
                        }
                    }

                    // Validate that all checkpoint IDs are contiguous
                    for (size_t i = 1; i < evts.size(); ++i)
                    {
                        if (evts[i].getCheckpointId() != evts[i - 1].getCheckpointId() + 1)
                        {
                            throw simdb::DBException("Could not validate checkpoint IDs");
                        }
                    }

                    SerializedEvtsBuffer serialized;
                    serialized.start_euid = evts.front().getEuid();
                    serialized.end_euid = evts.back().getEuid();
                    serialized.core_id = core_id_;
                    serialized.hart_id = hart_id_;

                    namespace bio = boost::iostreams;
                    bio::back_insert_device<std::vector<char>> inserter(serialized.evt_bytes);
                    bio::stream<bio::back_insert_device<std::vector<char>>> os(inserter);
                    boost::archive::binary_oarchive oa(os);
                    oa << evts;
                    os.flush();

                    out.emplace(std::move(serialized));
                    return simdb::pipeline::RunnableOutcome::DID_WORK;
                });

        // Task 2: zlib compress the serialized event buffers
        auto zlib = simdb::pipeline::createTask<
            simdb::pipeline::Function<SerializedEvtsBuffer, SerializedEvtsBuffer>>(
            [](SerializedEvtsBuffer && evts, simdb::ConcurrentQueue<SerializedEvtsBuffer> & out,
               bool /*force_flush*/)
            {
                std::vector<char> compressed_bytes;
                simdb::compressData(evts.evt_bytes, compressed_bytes);
                std::swap(evts.evt_bytes, compressed_bytes);
                out.emplace(std::move(evts));
                return simdb::pipeline::RunnableOutcome::DID_WORK;
            });

        // Task 3: Write the compressed event buffers to the database
        auto db_writer =
            db_accessor->createAsyncWriter<CoSimEventPipeline, SerializedEvtsBuffer, void>(
                [](SerializedEvtsBuffer && serialized, simdb::pipeline::AppPreparedINSERTs* tables,
                   bool /*force_flush*/)
                {
                    auto inserter = tables->getPreparedINSERT("CompressedEvents");
                    inserter->setColumnValue(0, serialized.start_euid);
                    inserter->setColumnValue(1, serialized.end_euid);
                    inserter->setColumnValue(2, serialized.core_id);
                    inserter->setColumnValue(3, serialized.hart_id);
                    inserter->setColumnValue(4, serialized.evt_bytes);
                    inserter->createRecord();
                    return simdb::pipeline::RunnableOutcome::DID_WORK;
                });

        // Connect the tasks
        *serialize >> *zlib >> *db_writer;

        // Store the head of the pipeline
        pipeline_head_ = serialize->getTypedInputQueue<EventList>();

        // Store a pipeline flusher
        pipeline_flusher_ = std::make_unique<simdb::pipeline::RunnableFlusher>(*db_mgr_, serialize,
                                                                               zlib, db_writer);

        // Attach pipeline task queue snoopers which are used to look for
        // an event directly from the pipeline. EventAccessor will look
        // here if an event is not found in the cache.
        pipeline_flusher_->assignQueueItemSnooper<EventList>(
            *serialize,
            [this](const EventList & evts) -> simdb::SnooperCallbackOutcome
            {
                auto euid = snooping_for_euid_.getValue();
                auto index = euid - evts.front().getEuid();
                if (index < evts.size())
                {
                    snooped_event_ = std::make_unique<Event>(evts[index]);
                    snooping_for_euid_.clearValid();
                    ++num_pipeline_evts_snooped_in_serialize_task_;
                    return simdb::SnooperCallbackOutcome::FOUND_STOP;
                }
                return simdb::SnooperCallbackOutcome::NOT_FOUND_CONTINUE;
            });

        pipeline_flusher_->assignQueueItemSnooper<SerializedEvtsBuffer>(
            *zlib,
            [this](const SerializedEvtsBuffer & evts) -> simdb::SnooperCallbackOutcome
            {
                auto euid = snooping_for_euid_.getValue();
                if (euid < evts.start_euid || euid > evts.end_euid)
                {
                    return simdb::SnooperCallbackOutcome::NOT_FOUND_CONTINUE;
                }

                // Undo boost::serialization
                namespace bio = boost::iostreams;
                bio::array_source src(evts.evt_bytes.data(), evts.evt_bytes.size());
                bio::stream<bio::array_source> is(src);
                boost::archive::binary_iarchive ia(is);

                EventList orig_evts;
                ia >> orig_evts;

                auto index = euid - orig_evts.front().getEuid();
                sparta_assert(index < orig_evts.size(),
                              "Event with euid " + std::to_string(euid)
                                  + " not found in serialized event buffer!");

                snooped_event_ = std::make_unique<Event>(orig_evts[index]);
                snooping_for_euid_.clearValid();
                ++num_pipeline_evts_snooped_in_zlib_task_;
                return simdb::SnooperCallbackOutcome::FOUND_STOP;
            });

        pipeline_flusher_->assignQueueItemSnooper<SerializedEvtsBuffer>(
            *db_writer,
            [this](const SerializedEvtsBuffer & evts) -> simdb::SnooperCallbackOutcome
            {
                auto euid = snooping_for_euid_.getValue();
                if (euid < evts.start_euid || euid > evts.end_euid)
                {
                    return simdb::SnooperCallbackOutcome::NOT_FOUND_CONTINUE;
                }

                // Undo zlib compression
                std::vector<char> uncompressed_bytes;
                simdb::decompressData(evts.evt_bytes, uncompressed_bytes);

                // Undo boost::serialization
                namespace bio = boost::iostreams;
                bio::array_source src(uncompressed_bytes.data(), uncompressed_bytes.size());
                bio::stream<bio::array_source> is(src);
                boost::archive::binary_iarchive ia(is);

                EventList orig_evts;
                ia >> orig_evts;

                auto index = euid - orig_evts.front().getEuid();
                sparta_assert(index < orig_evts.size(),
                              "Event with euid " + std::to_string(euid)
                                  + " not found in serialized event buffer!");

                snooped_event_ = std::make_unique<Event>(orig_evts[index]);
                snooping_for_euid_.clearValid();
                ++num_pipeline_evts_snooped_in_db_task_;
                return simdb::SnooperCallbackOutcome::FOUND_STOP;
            });

        // Put non-DB tasks on their own thread
        pipeline->createTaskGroup("EventPipeline")
            ->addTask(std::move(serialize))
            ->addTask(std::move(zlib));
    }

    simdb::pipeline::PipelineManager* CoSimEventPipeline::getPipelineManager()
    {
        return pipeline_mgr_;
    }

    void CoSimEventPipeline::setListener(EventListener* listener) { listener_ = listener; }

    void CoSimEventPipeline::onStep(Event && evt)
    {
        sparta_assert(core_id_ == evt.getCoreId() && hart_id_ == evt.getHartId(),
                      "Event core/hart ID does not match pipeline core/hart ID!");

        sparta_assert(evt.getEuid() == evt.getCheckpointId(),
                      "Event UID must match checkpoint ID in CoSimEventPipeline!");

        uncommitted_evts_buffer_.emplace_back(std::move(evt));
        last_event_uid_ = uncommitted_evts_buffer_.back().getEuid();

        if (listener_)
        {
            listener_->onNewEvent(getLastEvent());
        }
    }

    void CoSimEventPipeline::commitOldest()
    {
        sparta_assert(!uncommitted_evts_buffer_.empty(), "No uncommitted events to commit for core "
                                                             + std::to_string(core_id_) + ", hart "
                                                             + std::to_string(hart_id_) + "!");

        auto evt = std::move(uncommitted_evts_buffer_.front());
        uncommitted_evts_buffer_.pop_front();

        for (const auto & ext : evt.enabled_extensions_)
        {
            uncommitted_evts_enabled_extensions_baseline_.insert(ext);
        }
        for (const auto & ext : evt.disabled_extensions_)
        {
            uncommitted_evts_enabled_extensions_baseline_.erase(ext);
        }

        committed_evts_buffer_.emplace_back(std::move(evt));
        if (committed_evts_buffer_.size() == 100)
        {
            pipeline_head_->emplace(std::move(committed_evts_buffer_));
        }
    }

    void CoSimEventPipeline::commitUpTo(uint64_t euid)
    {
        sparta_assert(!uncommitted_evts_buffer_.empty(), "No uncommitted events to commit for core "
                                                             + std::to_string(core_id_) + ", hart "
                                                             + std::to_string(hart_id_) + "!");

        auto it = std::find_if(uncommitted_evts_buffer_.begin(), uncommitted_evts_buffer_.end(),
                               [euid](const Event & evt) { return evt.getEuid() == euid; });

        sparta_assert(it != uncommitted_evts_buffer_.end(),
                      "Could not find event with euid " + std::to_string(euid)
                          + " among uncommitted events for core " + std::to_string(core_id_)
                          + ", hart " + std::to_string(hart_id_) + "!");

        size_t num_to_commit = std::distance(uncommitted_evts_buffer_.begin(), std::next(it));
        while (num_to_commit-- > 0)
        {
            commitOldest();
        }
    }

    void CoSimEventPipeline::flush(uint64_t euid, bool flush_younger_only, CoSimObserver* observer,
                                   PegasusState* state)
    {
        // If the euid is <= last committed, we cannot flush it
        if (last_committed_event_uid_.isValid() && euid <= last_committed_event_uid_.getValue())
        {
            throw simdb::DBException("Cannot flush event with euid " + std::to_string(euid)
                                     + " for core " + std::to_string(core_id_) + ", hart "
                                     + std::to_string(hart_id_)
                                     + " because it has already been committed!");
        }

        // Throw if we have no uncommitted events
        if (uncommitted_evts_buffer_.empty())
        {
            throw simdb::DBException("Cannot flush event with euid " + std::to_string(euid)
                                     + " for core " + std::to_string(core_id_) + ", hart "
                                     + std::to_string(hart_id_)
                                     + " because there are no uncommitted events!");
        }

        // Event uids are contiguous, so quickly ensure the euid is within range
        if (euid < uncommitted_evts_buffer_.front().getEuid()
            || euid > uncommitted_evts_buffer_.back().getEuid())
        {
            throw simdb::DBException("Could not find event with euid " + std::to_string(euid)
                                     + " among uncommitted events for core "
                                     + std::to_string(core_id_) + ", hart "
                                     + std::to_string(hart_id_) + "!");
        }

        // Since we are reverting state changes, keep track of the extensions that
        // were enabled/disabled for all the events we are about to flush.
        std::unordered_set<std::string> post_flush_enabled_extensions;
        std::unordered_set<std::string> post_flush_disabled_extensions;

        // Find the event among uncommitted events and erase as needed
        auto it = std::find_if(uncommitted_evts_buffer_.begin(), uncommitted_evts_buffer_.end(),
                               [euid](const Event & evt) { return evt.getEuid() == euid; });

        if (flush_younger_only)
        {
            auto next_it = std::next(it);
            if (next_it != uncommitted_evts_buffer_.end())
            {
                state->setPc(next_it->getPc());
            }

            for (auto it2 = std::next(it); it2 != uncommitted_evts_buffer_.end(); ++it2)
            {
                for (const auto & ext : it2->enabled_extensions_)
                {
                    post_flush_disabled_extensions.insert(ext);
                }
                for (const auto & ext : it2->disabled_extensions_)
                {
                    post_flush_enabled_extensions.insert(ext);
                }
            }

            // Erase all events younger than the given euid
            uncommitted_evts_buffer_.erase(std::next(it), uncommitted_evts_buffer_.end());
        }
        else
        {
            state->setPc(it->getPc());

            for (auto it2 = it; it2 != uncommitted_evts_buffer_.end(); ++it2)
            {
                for (const auto & ext : it2->enabled_extensions_)
                {
                    post_flush_disabled_extensions.insert(ext);
                }
                for (const auto & ext : it2->disabled_extensions_)
                {
                    post_flush_enabled_extensions.insert(ext);
                }
            }

            // Erase all events up to and including the given euid
            uncommitted_evts_buffer_.erase(it, uncommitted_evts_buffer_.end());
        }

        // Reload state to the youngest uncommitted event. If there are none left,
        // then reload to the last committed event.
        sparta::utils::ValidValue<uint64_t> reload_euid;
        if (!uncommitted_evts_buffer_.empty())
        {
            reload_euid = uncommitted_evts_buffer_.back().getEuid();
        }
        else if (last_committed_event_uid_.isValid())
        {
            reload_euid = last_committed_event_uid_.getValue();
        }
        else
        {
            // Not sure what to do. Throw for now.
            throw simdb::DBException("After flushing event with euid " + std::to_string(euid)
                                     + " for core " + std::to_string(core_id_) + ", hart "
                                     + std::to_string(hart_id_)
                                     + ", there are no uncommitted or committed events to "
                                       "reload state from!");
        }

        sparta_assert(reload_euid.getValue() <= observer->event_uid_,
                      "Cannot reload state for event uid " + std::to_string(reload_euid.getValue())
                          + " since current event uid is " + std::to_string(observer->event_uid_)
                          + "!");

        auto reload_event = [&](const Event & reload_evt)
        {
            auto euid = reload_evt.getEuid();
            auto checkpointer = observer->getCheckpointer();
            checkpointer->loadCheckpoint(euid);

            last_event_uid_ = euid;
            observer->event_uid_ = euid;
            state->setPrivMode(reload_evt.getNextPrivilegeMode(), state->getVirtualMode());

            auto sim_state = state->getSimState();
            sim_state->reset();
            sim_state->current_opcode = reload_evt.getOpcode();
            sim_state->current_uid = reload_evt.getEuid();
            sim_state->sim_stopped = reload_evt.isLastEvent();
            sim_state->inst_count = reload_evt.getEuid();

            auto inst = state->getMavis()->makeInst(reload_evt.getOpcode(), state);
            inst->updateVecConfig(state);
            state->setCurrentInst(inst);
        };

        // Restore the enabled extensions before reloading any events from our uncommitted buffer.
        auto & ext_mgr = state_->getExtensionManager();
        const auto & curr_exts = ext_mgr.getEnabledExtensions();

        // Set flag which says not to update the baseline extensions. Our callback gets
        // hit for every enableExtension/disableExtension call otherwise.
        struct FlushingGuard
        {
            FlushingGuard(bool & flushing_flag) : flag_(flushing_flag)
            {
                flag_ = true;
            }
            ~FlushingGuard() { flag_ = false; }

         private:
            bool & flag_;
        } guard(flushing_);

        for (const auto & [ext_name, _] : curr_exts)
        {
            if (curr_exts.isEnabled(ext_name)
                && uncommitted_evts_enabled_extensions_baseline_.count(ext_name) == 0)
            {
                ext_mgr.disableExtension(ext_name);
            }

            else if (!curr_exts.isEnabled(ext_name)
                     && uncommitted_evts_enabled_extensions_baseline_.count(ext_name) > 0)
            {
                ext_mgr.enableExtension(ext_name);
            }
        }

        reload_event(uncommitted_evts_buffer_.back());

        // Undo extension changes for all flushed events
        for (const auto & ext : post_flush_enabled_extensions)
        {
            ext_mgr.enableExtension(ext);
        }
        for (const auto & ext : post_flush_disabled_extensions)
        {
            ext_mgr.disableExtension(ext);
        }

        // Switch contexts if extensions changed
        if (!post_flush_enabled_extensions.empty()
            || !post_flush_disabled_extensions.empty())
        {
            state->changeMavisContext();
        }

        // Refresh MMU mode (doesn't hurt even if not changed)
        if (observer->getRegWidth() == 8)
        {
            state->changeMMUMode<uint32_t>();
        }
        else
        {
            state->changeMMUMode<uint64_t>();
        }
    }

    uint64_t CoSimEventPipeline::getLastEventUID() const { return last_event_uid_; }

    EventAccessor CoSimEventPipeline::getLastEvent()
    {
        return EventAccessor(last_event_uid_, core_id_, hart_id_, this);
    }

    EventAccessor CoSimEventPipeline::getEvent(uint64_t euid)
    {
        return EventAccessor(euid, core_id_, hart_id_, this);
    }

    const EventList & CoSimEventPipeline::getUncommittedEvents() const
    {
        return uncommitted_evts_buffer_;
    }

    EventAccessor CoSimEventPipeline::getLastCommittedEvent() const
    {
        auto euid = last_committed_event_uid_.getValue();
        auto pipeline = const_cast<CoSimEventPipeline*>(this);
        return EventAccessor(euid, core_id_, hart_id_, pipeline);
    }

    void CoSimEventPipeline::stopSim()
    {
        sparta_assert(!uncommitted_evts_buffer_.empty());

        auto & last_evt = uncommitted_evts_buffer_.back();
        last_evt.event_ends_sim_ = true;
        last_evt.done_ = true;

        sim_stopped_ = true;
    }

    bool CoSimEventPipeline::simStopped() const { return sim_stopped_; }

    void CoSimEventPipeline::preTeardown()
    {
        if (!committed_evts_buffer_.empty())
        {
            pipeline_head_->emplace(std::move(committed_evts_buffer_));
        }

        if (!uncommitted_evts_buffer_.empty())
        {
            std::cout << "At sim end, event pipeline at core " << core_id_ << ", hart " << hart_id_
                      << " had " << uncommitted_evts_buffer_.size() << " uncommitted events.\n";
        }
    }

    void CoSimEventPipeline::postTeardown()
    {
        std::cout << "Event accesses for core " << core_id_ << ", hart " << hart_id_ << ":\n";
        std::cout << "    From cache: " << num_evts_retrieved_from_cache_ << "\n";

        if (avg_us_recreating_evts_from_pipeline_.count())
        {
            auto avg_latency_us = avg_us_recreating_evts_from_pipeline_.mean();
            std::cout << "    From pipeline:  " << avg_us_recreating_evts_from_pipeline_.count();
            std::cout << " (avg latency " << size_t(avg_latency_us) << " microseconds)\n";
            std::cout << "        From serialize task queue:  "
                      << num_pipeline_evts_snooped_in_serialize_task_ << "\n";
            std::cout << "        From zlib task queue:       "
                      << num_pipeline_evts_snooped_in_zlib_task_ << "\n";
            std::cout << "        From DB task queue:         "
                      << num_pipeline_evts_snooped_in_db_task_ << "\n";
        }
        else
        {
            std::cout << "    From pipeline:  0\n";
        }

        if (avg_us_recreating_evts_from_disk_.count())
        {
            auto avg_latency_us = avg_us_recreating_evts_from_disk_.mean();
            std::cout << "    From disk:  " << avg_us_recreating_evts_from_disk_.count();
            std::cout << " (avg latency " << size_t(avg_latency_us) << " microseconds)\n\n";
        }
        else
        {
            std::cout << "    From disk:  0\n\n";
        }
    }

    size_t CoSimEventPipeline::getNumCached() const
    {
        // We consider anything not yet sent to the pipeline as cached
        return uncommitted_evts_buffer_.size() + committed_evts_buffer_.size();
    }

    size_t CoSimEventPipeline::getNumSnooped() const
    {
        return num_pipeline_evts_snooped_in_serialize_task_
               + num_pipeline_evts_snooped_in_zlib_task_ + num_pipeline_evts_snooped_in_db_task_;
    }

    const Event* CoSimEventPipeline::getEventFromCache_(uint64_t euid)
    {
        auto get_from = [euid](const EventList & evts) -> const Event*
        {
            if (!evts.empty())
            {
                auto index = euid - evts.front().getEuid();
                if (index < evts.size())
                {
                    return &evts[index];
                }
            }

            return nullptr;
        };

        if (const Event* evt = get_from(uncommitted_evts_buffer_))
        {
            ++num_evts_retrieved_from_cache_;
            return evt;
        }

        if (const Event* evt = get_from(committed_evts_buffer_))
        {
            ++num_evts_retrieved_from_cache_;
            return evt;
        }

        return nullptr;
    }

    std::unique_ptr<Event> CoSimEventPipeline::recreateEventFromPipeline_(uint64_t euid)
    {
        // Keep track of how long we spend recreating events from the pipeline
        auto start = std::chrono::high_resolution_clock::now();

        snooping_for_euid_ = euid;
        auto outcome = pipeline_flusher_->snoopAll();
        if (outcome.found())
        {
            sparta_assert(snooped_event_ != nullptr);
            sparta_assert(!snooping_for_euid_.isValid());

            // Record how long this took
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> dur = end - start;
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

            // Add to the running mean
            avg_us_recreating_evts_from_pipeline_.add(us);

            return std::move(snooped_event_);
        }

        snooping_for_euid_.clearValid();
        return nullptr;
    }

    std::unique_ptr<Event> CoSimEventPipeline::recreateEventFromDisk_(uint64_t euid)
    {
        std::vector<char> compressed_evts_bytes;

        auto query_func = [&](simdb::DatabaseManager* db_mgr)
        {
            auto query = db_mgr->createQuery("CompressedEvents");
            query->addConstraintForUInt64("StartEuid", simdb::Constraints::LESS_EQUAL, euid);
            query->addConstraintForUInt64("EndEuid", simdb::Constraints::GREATER_EQUAL, euid);
            query->addConstraintForInt("CoreId", simdb::Constraints::EQUAL, (int)core_id_);
            query->addConstraintForInt("HartId", simdb::Constraints::EQUAL, (int)hart_id_);
            query->select("ZlibBlob", compressed_evts_bytes);

            auto result_set = query->getResultSet();
            result_set.getNextRecord();
        };

        // Keep track of how long we spend recreating events from disk
        auto start = std::chrono::high_resolution_clock::now();

        // Run the query on the database thread. It will stop what it is doing
        // as quickly as possible to run this query. The eval() method blocks
        // until the query is picked up and run.
        async_eval_->eval(query_func);

        if (compressed_evts_bytes.empty())
        {
            return nullptr;
        }

        // "Undo" the pipeline transforms. Start by undoing zlib.
        std::vector<char> uncompressed_evts_bytes;
        simdb::decompressData(compressed_evts_bytes, uncompressed_evts_bytes);

        // Now undo boost::serialization
        namespace bio = boost::iostreams;
        bio::array_source src(uncompressed_evts_bytes.data(), uncompressed_evts_bytes.size());
        bio::stream<bio::array_source> is(src);
        boost::archive::binary_iarchive ia(is);

        EventList evts;
        ia >> evts;

        // If we got this far, the event uid must be within the returned list
        if (euid < evts.front().getEuid())
        {
            throw simdb::DBException("Internal error occurred. Cannot find event with uid ")
                << euid << ".";
        }

        auto index = euid - evts.front().getEuid();
        if (index >= evts.size())
        {
            throw simdb::DBException("Internal error occurred. Cannot find event with uid ")
                << euid << ".";
        }

        // Record how long this took
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dur = end - start;
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

        // Add to the running mean
        avg_us_recreating_evts_from_disk_.add(us);

        // Return the requested event
        return std::make_unique<Event>(evts[index]);
    }

    const Event* EventAccessor::operator->() { return get(); }

    const Event* EventAccessor::get()
    {
        if (recreated_evt_)
        {
            return recreated_evt_.get();
        }

        if (auto evt = evt_pipeline_->getEventFromCache_(euid_))
        {
            ++num_from_cache_;
            return evt;
        }

        // Disable pipeline tasks while we try to find the event from the
        // pipeline, or falling back to running a DB query. The tasks will
        // be re-enabled when this object goes out of scope.
        auto disabler = evt_pipeline_->getPipelineManager()->scopedDisableAll();

        if (auto evt = evt_pipeline_->recreateEventFromPipeline_(euid_))
        {
            ++num_from_pipeline_;
            recreated_evt_ = std::move(evt);
            return recreated_evt_.get();
        }

        if (auto evt = evt_pipeline_->recreateEventFromDisk_(euid_))
        {
            ++num_from_disk_;
            recreated_evt_ = std::move(evt);
            return recreated_evt_.get();
        }

        throw simdb::DBException("Unable to find the event with euid ") << euid_;
    }

} // namespace pegasus::cosim

#include "simdb/apps/AppRegistration.hpp"
REGISTER_SIMDB_APPLICATION(pegasus::cosim::CoSimEventPipeline);
