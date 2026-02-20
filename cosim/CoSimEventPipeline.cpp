#include "cosim/CoSimEventPipeline.hpp"
#include "core/observers/CoSimObserver.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusCore.hpp"
#include "core/Execute.hpp"
#include "simdb/pipeline/PipelineManager.hpp"
#include "simdb/pipeline/AsyncDatabaseAccessor.hpp"
#include "simdb/pipeline/Stage.hpp"
#include "simdb/utils/Compress.hpp"
#include "sparta/serialization/checkpoint/CherryPickFastCheckpointer.hpp"
#include "source/include/softfloat.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>

namespace pegasus::cosim
{

    CoSimEventPipeline::CoSimEventPipeline(simdb::DatabaseManager* db_mgr, CoreId core_id,
                                           HartId hart_id, PegasusState* state) :
        db_mgr_(db_mgr),
        core_id_(core_id),
        hart_id_(hart_id),
        state_(state)
    {
        auto & ext_mgr = state->getCore()->getExtensionManager();

        ext_mgr.registerExtensionChangeCallback(
            [this](const std::vector<std::string> & extensions, bool enabled)
            {
                if (flushing_)
                {
                    return;
                }

                sparta_assert(observer_ != nullptr, "CoSimObserver never set!");
                Event & last_event = observer_->last_event_;
                last_event.extension_changes_.emplace_back(extensions, enabled);
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

        // Event UID range covered by this blob.
        // There is no overlap in UIDs between blobs.
        // These aren't necessarily contiguous (and won't
        // be if any events were flushed).
        tbl.addColumn("StartEuid", dt::uint64_t);
        tbl.addColumn("EndEuid", dt::uint64_t);

        // Contiguous arch ID range covered by this blob
        tbl.addColumn("StartArchId", dt::uint64_t);
        tbl.addColumn("EndArchId", dt::uint64_t);

        // Remember that all cores/harts share the same database,
        // so we need to distinguish events by core/hart ID.
        tbl.addColumn("CoreId", dt::uint32_t);
        tbl.addColumn("HartId", dt::uint32_t);

        // The compressed event data blob
        tbl.addColumn("ZlibBlob", dt::blob_t);

        // Index for fast lookup by euid/arch ranges and core/hart ID.
        tbl.createCompoundIndexOn(
            {"StartEuid", "EndEuid", "StartArchId", "EndArchId", "CoreId", "HartId"});
        tbl.unsetPrimaryKey();
    }

    class EventCompressorStage : public simdb::pipeline::Stage
    {
      public:
        using SerializedEvtsBuffer = CoSimEventPipeline::SerializedEvtsBuffer;

        EventCompressorStage(CoSimEventPipeline* pipeline) : pipeline_(pipeline)
        {
            addInPort_<EventList>("events_in", input_queue_);
            addOutPort_<SerializedEvtsBuffer>("compressed_events_out", output_queue_);
        }

        bool snoop(const uint64_t & euid, std::unique_ptr<Event> & snooped_event)
        {
            sparta_assert(snooped_event == nullptr);

            // Look through the EventList input queue feeding this stage
            input_queue_->snoop(
                [&](const EventList & evts)
                {
                    // Note that the euids are not necessarily contiguous in the EventList,
                    // so we have to iterate through to find the event with the given euid.
                    // The only time these euids will be contiguous is when we run CoSim
                    // without EVER flushing events.
                    //
                    // That being said, it is still quick enough to calculate the index
                    // and then check that event's euid first before iterating through the
                    // entire list.
                    auto index = euid - evts.front().getEuid();
                    if (index < evts.size() && evts[index].getEuid() == euid)
                    {
                        // Found it! Make a copy.
                        snooped_event = std::make_unique<Event>(evts[index]);

                        // Let the ConcurrentQueue know that we can stop iterating the queue.
                        // This lambda is called for every queue item until we return true.
                        return true;
                    }

                    // Looks like we have to iterate through the entire list.
                    for (const auto & evt : evts)
                    {
                        if (evt.getEuid() == euid)
                        {
                            // Found it! Make a copy.
                            snooped_event = std::make_unique<Event>(evt);

                            // Let the ConcurrentQueue know that we can stop iterating the queue.
                            // This lambda is called for every queue item until we return true.
                            return true;
                        }
                    }

                    // Keep going.
                    return false;
                });

            if (snooped_event != nullptr)
            {
                // Increment the appropriate counter for reporting purposes.
                ++pipeline_->num_pipeline_evts_snooped_in_serialize_queue_;

                // Let the caller know that 'snooped_event' is valid.
                return true;
            }

            // Let the caller know that 'snooped_event' is invalid.
            return false;
        }

      private:
        simdb::pipeline::PipelineAction run_(bool) override
        {
            auto action = simdb::pipeline::PipelineAction::SLEEP;

            EventList evts;
            if (input_queue_->try_pop(evts))
            {
                // Validate all core/hart IDs match our expected IDs
                for (const auto & evt : evts)
                {
                    sparta_assert(evt.getCoreId() == pipeline_->core_id_);
                    sparta_assert(evt.getHartId() == pipeline_->hart_id_);
                }

                // Assign auto-incrementing arch IDs to each event
                for (auto & evt : evts)
                {
                    evt.arch_id_ = arch_id_++;
                }

                auto start_euid = UINT64_MAX;
                auto end_euid = 0ULL;
                for (const auto & evt : evts)
                {
                    if (evt.getEuid() < start_euid)
                    {
                        start_euid = evt.getEuid();
                    }
                    if (evt.getEuid() > end_euid)
                    {
                        end_euid = evt.getEuid();
                    }
                }

                // Serialize the events to a byte buffer
                SerializedEvtsBuffer serialized;
                serialized.start_euid = start_euid;
                serialized.end_euid = end_euid;
                serialized.start_arch_id = evts.front().getArchId();
                serialized.end_arch_id = evts.back().getArchId();
                serialized.core_id = pipeline_->core_id_;
                serialized.hart_id = pipeline_->hart_id_;

                namespace bio = boost::iostreams;
                bio::back_insert_device<std::vector<char>> inserter(serialized.evt_bytes);
                bio::stream<bio::back_insert_device<std::vector<char>>> os(inserter);
                boost::archive::binary_oarchive oa(os);
                oa << evts;
                os.flush();

                // Compress the byte buffer
                std::vector<char> compressed_bytes;
                simdb::compressData(serialized.evt_bytes, compressed_bytes);
                std::swap(serialized.evt_bytes, compressed_bytes);

                // Send down the pipeline
                output_queue_->emplace(std::move(serialized));
                action = simdb::pipeline::PipelineAction::PROCEED;
            }

            return action;
        }

        CoSimEventPipeline* pipeline_ = nullptr;
        simdb::ConcurrentQueue<EventList>* input_queue_ = nullptr;
        simdb::ConcurrentQueue<SerializedEvtsBuffer>* output_queue_ = nullptr;
        uint64_t arch_id_ = 1;
    };

    class EventWriterStage : public simdb::pipeline::DatabaseStage<CoSimEventPipeline>
    {
      public:
        using SerializedEvtsBuffer = CoSimEventPipeline::SerializedEvtsBuffer;

        EventWriterStage(CoSimEventPipeline* pipeline) : pipeline_(pipeline)
        {
            addInPort_<SerializedEvtsBuffer>("compressed_events_in", input_queue_);
        }

        bool snoop(const uint64_t & euid, std::unique_ptr<Event> & snooped_event)
        {
            sparta_assert(snooped_event == nullptr);

            // Look through the SerializedEvtsBuffer input queue feeding this stage
            input_queue_->snoop(
                [&](const SerializedEvtsBuffer & evts)
                {
                    if (euid < evts.start_euid || euid > evts.end_euid)
                    {
                        return false;
                    }

                    // Undo zlib
                    std::vector<char> uncompressed;
                    simdb::decompressData(evts.evt_bytes, uncompressed);

                    // Undo boost::serialization
                    namespace bio = boost::iostreams;
                    bio::array_source src(uncompressed.data(), uncompressed.size());
                    bio::stream<bio::array_source> is(src);
                    boost::archive::binary_iarchive ia(is);

                    EventList orig_evts;
                    ia >> orig_evts;

                    // Note that the euids are not necessarily contiguous in the EventList,
                    // so we have to iterate through to find the event with the given euid.
                    // The only time these euids will be contiguous is when we run CoSim
                    // without EVER flushing events.
                    //
                    // That being said, it is still quick enough to calculate the index
                    // and then check that event's euid first before iterating through the
                    // entire list.
                    auto index = euid - orig_evts.front().getEuid();
                    if (index < orig_evts.size() && orig_evts[index].getEuid() == euid)
                    {
                        // Found it! Make a copy.
                        snooped_event = std::make_unique<Event>(orig_evts[index]);
                    }

                    else
                    {
                        // Looks like we have to iterate through the entire list.
                        for (const auto & evt : orig_evts)
                        {
                            if (evt.getEuid() == euid)
                            {
                                // Found it! Make a copy.
                                snooped_event = std::make_unique<Event>(evt);
                                break;
                            }
                        }
                    }

                    if (snooped_event == nullptr)
                    {
                        // We should NEVER get here! We already verified that the euid is within
                        // range.
                        throw simdb::DBException("Internal error occurred - mismatching EUIDs");
                    }

                    // Increment the appropriate counter for reporting purposes.
                    return true;
                });

            if (snooped_event != nullptr)
            {
                // Increment the appropriate counter for reporting purposes.
                ++pipeline_->num_pipeline_evts_snooped_in_db_queue_;

                // Let the caller know that 'snooped_event' is valid.
                return true;
            }

            // Let the caller know that 'snooped_event' is invalid.
            return false;
        }

      private:
        simdb::pipeline::PipelineAction run_(bool) override
        {
            auto action = simdb::pipeline::PipelineAction::SLEEP;

            SerializedEvtsBuffer serialized;
            if (input_queue_->try_pop(serialized))
            {
                auto inserter = getTableInserter_("CompressedEvents");
                inserter->setColumnValue(0, serialized.start_euid);
                inserter->setColumnValue(1, serialized.end_euid);
                inserter->setColumnValue(2, serialized.start_arch_id);
                inserter->setColumnValue(3, serialized.end_arch_id);
                inserter->setColumnValue(4, serialized.core_id);
                inserter->setColumnValue(5, serialized.hart_id);
                inserter->setColumnValue(6, serialized.evt_bytes);
                inserter->createRecord();
                action = simdb::pipeline::PipelineAction::PROCEED;
            }

            return action;
        }

        simdb::ConcurrentQueue<SerializedEvtsBuffer>* input_queue_ = nullptr;
        CoSimEventPipeline* pipeline_ = nullptr;
    };

    void CoSimEventPipeline::createPipeline(simdb::pipeline::PipelineManager* pipeline_mgr)
    {
        auto pipeline = pipeline_mgr->createPipeline(NAME, this);

        auto compressor = pipeline->addStage<EventCompressorStage>("compress_events", this);
        auto db_writer = pipeline->addStage<EventWriterStage>("write_events", this);
        pipeline->noMoreStages();

        pipeline->bind("compress_events.compressed_events_out",
                       "write_events.compressed_events_in");
        pipeline->noMoreBindings();

        // Store the head of the pipeline
        pipeline_head_ = pipeline->getInPortQueue<EventList>("compress_events.events_in");

        // Store a pipeline flusher
        pipeline_flusher_ = pipeline->createFlusher({"compress_events", "write_events"});

        // Store the pipeline manager so we can run async DB queries from the main thread
        pipeline_mgr_ = pipeline_mgr;

        // Configure the pipeline snooper so we can look for events directly in the pipeline
        // instead of needing to go to the database when the event is not in the cache.
        pipeline_snooper_ = pipeline_mgr->createSnooper<uint64_t, std::unique_ptr<Event>>();
        pipeline_snooper_->addStage(compressor);
        pipeline_snooper_->addStage(db_writer);
    }

    simdb::pipeline::PipelineManager* CoSimEventPipeline::getPipelineManager() const
    {
        return pipeline_mgr_;
    }

    void CoSimEventPipeline::setListener(EventListener* listener) { listener_ = listener; }

    void CoSimEventPipeline::onStep(Event && evt)
    {
        sparta_assert(core_id_ == evt.getCoreId() && hart_id_ == evt.getHartId(),
                      "Event core/hart ID does not match pipeline core/hart ID!");

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
        last_committed_event_uid_ = evt.getEuid();

        committed_evts_buffer_.emplace_back(std::move(evt));
        if (committed_evts_buffer_.size() == 100)
        {
            pipeline_head_->emplace(std::move(committed_evts_buffer_));
            observer_->getCheckpointer()->commitCurrentBranch();
        }

        // Handle syscall emulation
        if (state_->getCore()->isSystemCallEmulationEnabled() && (evt.getOpcode() == ECALL_OPCODE))
        {
            sparta_assert(
                uncommitted_evts_buffer_.empty(),
                "Cannot emulate a system call with uncommitted Events! Num uncommitted events: "
                    << uncommitted_evts_buffer_.size());
            if (state_->getXlen() == 64)
            {
                const RV64 ret_code = state_->emulateSystemCall<RV64>();
                WRITE_INT_REG<RV64>(state_, 10, ret_code);
            }
            else
            {
                const RV32 ret_code = state_->emulateSystemCall<RV32>();
                WRITE_INT_REG<RV32>(state_, 10, ret_code);
            }
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

        // Set flag which says not to update the baseline extensions. Our callback gets
        // hit for every enableExtension/disableExtension call otherwise.
        struct FlushingGuard
        {
            FlushingGuard(bool & flushing_flag) : flag_(flushing_flag) { flag_ = true; }

            ~FlushingGuard() { flag_ = false; }

          private:
            bool & flag_;
        } guard(flushing_);

        auto & ext_mgr = state_->getCore()->getExtensionManager();

        // Undo the CSR side effects, softfloat changes, MMU mode, and extension changes
        // for every uncommitted event we are flushing. All events will be undone in the
        // reverse order they occurred (undo youngest to oldest).
        auto change_mmu_mode = false;

        auto undo_evt = [&](const Event & evt) -> bool
        {
            if (flush_younger_only && evt.getEuid() == euid)
            {
                return false;
            }

            if (!flush_younger_only && evt.getEuid() < euid)
            {
                return false;
            }

            state->setPc(evt.getPc());
            state->setPrivMode(evt.getPrivilegeMode(), state->getVirtualMode());
            state->setCurrentException(evt.getPrevExceptionCode());

            if (evt.getStartReservation().isValid())
            {
                state->getCore()->getReservation(hart_id_) = evt.getStartReservation();
            }
            else
            {
                state->getCore()->getReservation(hart_id_).clearValid();
            }

            softfloat_roundingMode = evt.start_softfloat_flags_.softfloat_roundingMode;
            softfloat_detectTininess = evt.start_softfloat_flags_.softfloat_detectTininess;
            softfloat_exceptionFlags = evt.start_softfloat_flags_.softfloat_exceptionFlags;
            extF80_roundingPrecision = evt.start_softfloat_flags_.extF80_roundingPrecision;

            std::vector<std::string> exts_to_enable;
            std::vector<std::string> exts_to_disable;
            const auto & ext_changes = evt.extension_changes_;
            for (auto rit = ext_changes.rbegin(); rit != ext_changes.rend(); ++rit)
            {
                const auto & ext_info = *rit;
                const auto & ext_names = ext_info.extensions;
                const auto enabled = ext_info.enabled;

                if (enabled)
                {
                    exts_to_disable.insert(exts_to_disable.end(), ext_names.begin(),
                                           ext_names.end());
                }
                else
                {
                    exts_to_enable.insert(exts_to_enable.end(), ext_names.begin(), ext_names.end());
                }
            }

            exts_to_enable.erase(std::unique(exts_to_enable.begin(), exts_to_enable.end()),
                                 exts_to_enable.end());
            exts_to_disable.erase(std::unique(exts_to_disable.begin(), exts_to_disable.end()),
                                  exts_to_disable.end());

            if (!exts_to_enable.empty() || !exts_to_disable.empty())
            {
                ext_mgr.changeExtensions(exts_to_enable, exts_to_disable);
                state->getCore()->changeMavisContext();
            }

            // Update MMU mode when MSTATUS / SATP csrs have changed,
            // or when the privilege mode(s) have changed.
            change_mmu_mode |= evt.curr_priv_ != evt.next_priv_;
            change_mmu_mode |= evt.curr_ldst_priv_ != evt.next_ldst_priv_;
            if (!change_mmu_mode && evt.inst_csr_ != std::numeric_limits<uint32_t>::max())
            {
                change_mmu_mode |= evt.inst_csr_ == MSTATUS || evt.inst_csr_ == SSTATUS
                                   || evt.inst_csr_ == VSSTATUS || evt.inst_csr_ == HSTATUS;
                change_mmu_mode |=
                    evt.inst_csr_ == SATP || evt.inst_csr_ == VSATP || evt.inst_csr_ == HGATP;
            }

            return true;
        };

        sparta_assert(!uncommitted_evts_buffer_.empty());
        while (undo_evt(uncommitted_evts_buffer_.back()))
        {
            uncommitted_evts_buffer_.pop_back();
            if (uncommitted_evts_buffer_.empty())
            {
                break;
            }
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

        auto reload_event = [&](const Event & reload_evt)
        {
            auto euid = reload_evt.getEuid();
            auto checkpointer = observer->getCheckpointer();
            checkpointer->getFastCheckpointer().loadCheckpoint(euid);

            last_event_uid_ = euid;
            sim_stopped_ = reload_evt.isLastEvent();

            state->setPc(reload_evt.getNextPc());
            state->setPrivMode(reload_evt.getNextPrivilegeMode(), state->getVirtualMode());

            if (reload_evt.getEndReservation().isValid())
            {
                state->getCore()->getReservation(hart_id_) = reload_evt.getEndReservation();
            }
            else
            {
                state->getCore()->getReservation(hart_id_).clearValid();
            }

            softfloat_roundingMode = reload_evt.end_softfloat_flags_.softfloat_roundingMode;
            softfloat_detectTininess = reload_evt.end_softfloat_flags_.softfloat_detectTininess;
            softfloat_exceptionFlags = reload_evt.end_softfloat_flags_.softfloat_exceptionFlags;
            extF80_roundingPrecision = reload_evt.end_softfloat_flags_.extF80_roundingPrecision;

            auto sim_state = state->getSimState();
            sim_state->reset();
            sim_state->current_opcode = reload_evt.getOpcode();
            sim_state->current_uid = reload_evt.getSimStateCurrentUID();
            sim_state->sim_stopped = reload_evt.isLastEvent();
            sim_state->inst_count = reload_evt.getSimStateCurrentUID();
            sim_state->test_passed = sim_state->workload_exit_code == 0;
            if (!sim_state->sim_stopped)
            {
                sim_state->workload_exit_code = 0;
            }

            // Now that the ArchData is reloaded, we can safely update the MMU mode.
            if (change_mmu_mode)
            {
                if (observer->getRegWidth() == 8)
                {
                    state->updateTranslationMode<uint32_t>(
                        translate_types::TranslationStage::SUPERVISOR);
                    state->updateTranslationMode<uint32_t>(
                        translate_types::TranslationStage::VIRTUAL_SUPERVISOR);
                    state->updateTranslationMode<uint32_t>(
                        translate_types::TranslationStage::GUEST);
                }
                else
                {
                    state->updateTranslationMode<uint64_t>(
                        translate_types::TranslationStage::SUPERVISOR);
                    state->updateTranslationMode<uint64_t>(
                        translate_types::TranslationStage::VIRTUAL_SUPERVISOR);
                    state->updateTranslationMode<uint64_t>(
                        translate_types::TranslationStage::GUEST);
                }
            }
        };

        if (!uncommitted_evts_buffer_.empty())
        {
            const auto & reload_evt = uncommitted_evts_buffer_.back();
            sparta_assert(reload_evt.getEuid() == reload_euid.getValue());
            reload_event(reload_evt);
        }
        else
        {
            auto reload_evt_accessor = getLastCommittedEvent();
            const auto & reload_evt = *reload_evt_accessor.get();
            sparta_assert(reload_evt.getEuid() == reload_euid.getValue());
            reload_event(reload_evt);
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
            std::cout << "WARNING: At sim end, event pipeline at core " << core_id_ << ", hart "
                      << hart_id_ << " had " << uncommitted_evts_buffer_.size()
                      << " uncommitted events.\n";
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
            std::cout << "        From serialize stage:  "
                      << num_pipeline_evts_snooped_in_serialize_queue_ << "\n";
            std::cout << "        From DB stage:         " << num_pipeline_evts_snooped_in_db_queue_
                      << "\n";
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

    size_t CoSimEventPipeline::getNumSnooped() const
    {
        return num_pipeline_evts_snooped_in_serialize_queue_
               + num_pipeline_evts_snooped_in_db_queue_;
    }

    size_t CoSimEventPipeline::getNumCached() const
    {
        return uncommitted_evts_buffer_.size() + committed_evts_buffer_.size();
    }

    const Event* CoSimEventPipeline::getEventFromCache_(uint64_t euid)
    {
        auto get_from = [euid](const EventList & evts) -> const Event*
        {
            if (!evts.empty())
            {
                auto index = euid - evts.front().getEuid();

                // Optimistically check the calculated index first (no flushes so far).
                if (index < evts.size() && evts[index].getEuid() == euid)
                {
                    return &evts[index];
                }

                // Iterate through the list to find the event (we have done some flushes).
                for (const auto & evt : evts)
                {
                    if (evt.getEuid() == euid)
                    {
                        return &evt;
                    }
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

        std::unique_ptr<Event> snooped_event;
        pipeline_snooper_->snoopAllStages(euid, snooped_event);

        if (!snooped_event)
        {
            return nullptr;
        }

        // Record how long this took
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dur = end - start;
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

        // Add to the running mean
        avg_us_recreating_evts_from_pipeline_.add(us);

        return snooped_event;
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
        auto db_accessor = pipeline_mgr_->getAsyncDatabaseAccessor();
        db_accessor->eval(query_func);

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
        for (const auto & evt : evts)
        {
            if (evt.getEuid() == euid)
            {
                // Record how long this took
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> dur = end - start;
                auto us = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

                // Add to the running mean
                avg_us_recreating_evts_from_disk_.add(us);

                return std::make_unique<Event>(evt);
            }
        }

        throw simdb::DBException("Internal error occurred. Cannot find event with uid ")
            << euid << ".";
    }

    const Event* EventAccessor::operator->() { return get(); }

    const Event* EventAccessor::get(bool must_exist)
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

        if (must_exist)
        {
            throw simdb::DBException("Unable to find the event with euid ") << euid_;
        }

        return nullptr;
    }

} // namespace pegasus::cosim
