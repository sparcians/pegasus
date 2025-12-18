#pragma once

#include "simdb/apps/App.hpp"
#include "simdb/utils/ConcurrentQueue.hpp"
#include "simdb/utils/RunningMean.hpp"
#include "simdb/pipeline/PipelineSnooper.hpp"
#include "simdb/Exceptions.hpp"
#include "cosim/EventAccessor.hpp"
#include "cosim/Event.hpp"
#include "cosim/CoSimApi.hpp"
#include <unordered_set>

namespace simdb::pipeline
{
    class Flusher;
}

namespace pegasus
{
    class PegasusState;
}

namespace pegasus::cosim
{
    class CoSimObserver;
    class EventCompressorStage;
    class EventWriterStage;

    /// Base class for event listener. Receives EventAccessors for
    /// every cosim step(). Used for testing and validation.
    class EventListener
    {
      public:
        virtual ~EventListener() = default;
        virtual void onNewEvent(EventAccessor && evt) = 0;
    };

    /// This class implements a performant pipeline to process
    /// cosim events. A series of transformations will occur on
    /// the events on a background thread on their way to the DB,
    /// and the pipeline has an API to access events via the
    /// EventAccessor class. This allows you to get access to
    /// an event which may live in the pipeline cache or on disk.
    ///
    /// Each CoSimObserver will get their own instance of this class
    /// along with their own Sparta CherryPickFastCheckpointer. The reason
    /// they need their own checkpointer is that one checkpointer
    /// is to be used with a specific PegasusState instance, which
    /// is per core / per hart. Each observer is tied 1-to-1 with
    /// PegasusState.
    ///
    /// Despite observers having their own pipeline instance, SimDB will
    /// share the same underlying database connection and DB/pipeline
    /// threads.
    class CoSimEventPipeline : public simdb::App
    {
      public:
        /// All SimDB apps must provide a NAME
        static constexpr auto NAME = "cosim-event-pipeline";

        /// Constructor must have this signature for simdb::AppManager factories
        CoSimEventPipeline(simdb::DatabaseManager* db_mgr, CoreId core_id, HartId hart_id,
                           PegasusState* state);

        /// Set the associated observer. Not added to the ctor to avoid circular
        /// dependencies between CoSimEventPipeline and CoSimObserver (each needs
        /// the other to be constructed).
        void setObserver(CoSimObserver* observer);

        /// Define the SimDB schema.
        static void defineSchema(simdb::Schema & schema);

        /// Instantiate the pipeline.
        void createPipeline(simdb::pipeline::PipelineManager* pipeline_mgr) override;

        /// Allow EventAccessor to use the PipelineManager to disable threads
        /// while recreating events as needed (when not found in cache).
        simdb::pipeline::PipelineManager* getPipelineManager() const;

        /// Called by unit tests to validate async event retrieval.
        void setListener(EventListener* listener);

        /// Process a new event from cosim step(). Called during postExecute().
        void onStep(Event && evt);

        /// Commit the oldest uncommitted event.
        void commitOldest();

        /// Commit all uncommitted events up to the given event uid.
        void commitUpTo(uint64_t euid);

        /// Flush the event with the given uid. If flush_younger_only=true,
        /// only flush younger uncommitted events. This method will throw if
        /// the event to flush has already been committed.
        void flush(uint64_t euid, bool flush_younger_only, CoSimObserver* observer,
                   PegasusState* state);

        /// Get the event uid for the most recent event for this core/hart.
        uint64_t getLastEventUID() const;

        /// Get an EventAccessor for the most recent event for this core/hart.
        EventAccessor getLastEvent();

        /// Create an EventAccessor for the given event uid for this core/hart.
        EventAccessor getEvent(uint64_t euid);

        /// Get the list of uncommitted events for this core/hart.
        const EventList & getUncommittedEvents() const;

        /// Get an EventAccessor for the most recent committed event for this core/hart.
        EventAccessor getLastCommittedEvent() const;

        /// Called by the CoSimObserver when Pegasus calls stopSim().
        void stopSim();

        /// Check if the sim has reached its stopSim().
        bool simStopped() const;

        /// Called before the pipeline is torn down. Send committed events down the pipeline if any.
        void preTeardown() override;

        /// Called after the pipeline threads have been flushed and destroyed.
        void postTeardown() override;

        /// Get the total number of events successfully snooped
        /// from the pipeline after not finding it in the cache.
        /// Returns the sum of snooped events from the serialize
        /// queue and the database queue.
        ///
        /// Used for testing only.
        size_t getNumSnooped() const;

        /// Get the total number of events still in the cache.
        /// Used for testing only.
        size_t getNumCached() const;

      private:
        /// Friend access given to EventAccessor for event retrieval.
        friend class EventAccessor;

        /// Return a pointer to the event in the cache. Not safe to use
        /// this pointer later as it may have been removed from the cache.
        const Event* getEventFromCache_(uint64_t euid);

        /// Recreate an old event from the pipeline when it is no longer in the cache.
        std::unique_ptr<Event> recreateEventFromPipeline_(uint64_t euid);

        /// Recreate an old event from disk when it is no longer in the cache.
        std::unique_ptr<Event> recreateEventFromDisk_(uint64_t euid);

        /// SimDB instance.
        simdb::DatabaseManager* db_mgr_ = nullptr;

        /// Pipeline manager. Used to disable the pipeline while we
        /// are looking for events in the pipeline or on disk. Also
        /// used to access the AsyncDatabaseAccessor.
        simdb::pipeline::PipelineManager* pipeline_mgr_ = nullptr;

        /// Core ID.
        const CoreId core_id_;

        /// Hart ID.
        const HartId hart_id_;

        /// PegasusState associated with this pipeline.
        PegasusState* const state_;

        /// CoSimObserver associated with this pipeline.
        CoSimObserver* observer_ = nullptr;

        /// Flag which prevents us from updating the enabled extensions baseline
        /// during a flush operation.
        bool flushing_ = false;

        /// Last seen event uid.
        sparta::utils::ValidValue<uint64_t> last_event_uid_;

        /// Retain the last committed event uid. We may be asked for the last
        /// committed event, but it may have already been sent to the pipeline.
        sparta::utils::ValidValue<uint64_t> last_committed_event_uid_;

        /// Buffer of events that are not yet committed. These are the ONLY events
        /// that can be used to perform a flush.
        EventList uncommitted_evts_buffer_;

        /// Buffer of events that have been committed, but not yet sent to the pipeline.
        /// Will be sent down the pipeline when full.
        EventList committed_evts_buffer_;

        /// First task input queue that accepts committed events.
        simdb::ConcurrentQueue<EventList>* pipeline_head_ = nullptr;

        /// Utility to flush the pipeline on demand.
        std::unique_ptr<simdb::pipeline::Flusher> pipeline_flusher_;

        /// Pipeline snooper used to look for events directly in the pipeline
        /// instead of needing to go to the database when the event is not in the cache.
        simdb::pipeline::PipelineSnooper<uint64_t, std::unique_ptr<Event>> pipeline_snooper_;

        /// Event recreated from the pipeline snoopers.
        std::unique_ptr<Event> snooped_event_;

        /// Has the simulation been stopped?
        bool sim_stopped_ = false;

        /// Metrics to print out usage and performance to stdout.
        size_t num_evts_retrieved_from_cache_ = 0;
        simdb::RunningMean avg_us_recreating_evts_from_disk_;
        simdb::RunningMean avg_us_recreating_evts_from_pipeline_;
        size_t num_pipeline_evts_snooped_in_serialize_queue_ = 0;
        size_t num_pipeline_evts_snooped_in_db_queue_ = 0;

        /// Listener which inspects events as they come through the pipeline.
        /// Used for testing and validation.
        EventListener* listener_ = nullptr;

        /// Structure which holds a range of events serialized to a byte buffer.
        struct SerializedEvtsBuffer
        {
            std::vector<char> evt_bytes;
            uint64_t start_euid = UINT64_MAX;
            uint64_t end_euid = UINT64_MAX;
            uint64_t start_arch_id = UINT64_MAX;
            uint64_t end_arch_id = UINT64_MAX;
            CoreId core_id = UINT32_MAX;
            HartId hart_id = UINT32_MAX;
        };

        friend class EventCompressorStage;
        friend class EventWriterStage;
    };

} // namespace pegasus::cosim

namespace simdb
{

    template <> class AppFactory<pegasus::cosim::CoSimEventPipeline> : public AppFactoryBase
    {
      public:
        using CoreId = pegasus::CoreId;
        using HartId = pegasus::HartId;
        using PegasusState = pegasus::PegasusState;
        using AppT = pegasus::cosim::CoSimEventPipeline;

        void setCtorArgs(size_t instance_num, CoreId core_id, HartId hart_id, PegasusState* state)
        {
            ctor_args_by_inst_num_[instance_num] = std::make_tuple(core_id, hart_id, state);
        }

        AppT* createApp(DatabaseManager* db_mgr, size_t instance_num = 0) override
        {
            auto it = ctor_args_by_inst_num_.find(instance_num);
            if (it == ctor_args_by_inst_num_.end())
            {
                throw DBException("Ctor args not set for CoSimEventPipeline instance "
                                  + std::to_string(instance_num));
            }

            const auto & [core_id, hart_id, state] = it->second;
            return new pegasus::cosim::CoSimEventPipeline(db_mgr, core_id, hart_id, state);
        }

        void defineSchema(Schema & schema) const override { AppT::defineSchema(schema); }

      private:
        using CtorArgs = std::tuple<CoreId, HartId, PegasusState*>;
        std::map<size_t, CtorArgs> ctor_args_by_inst_num_;
    };

} // namespace simdb
