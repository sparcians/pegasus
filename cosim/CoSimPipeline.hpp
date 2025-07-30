#pragma once

#include "simdb/apps/App.hpp"
#include "simdb/Exceptions.hpp"
#include "simdb/utils/ConcurrentQueue.hpp"
#include "simdb/utils/RunningMean.hpp"
#include "cosim/EventAccessor.hpp"
#include "cosim/Event.hpp"

#include <deque>

namespace pegasus::cosim
{
    using EuidRange = std::pair<uint64_t, uint64_t>;
    using EventBuffer = std::vector<Event>;

    /// This struct is used for optimizations in the cosim pipeline.
    struct EventsRange
    {
        EuidRange euid_range;
        EventBuffer events;

        /// Called to/from char buffer (boost::serialization)
        template <typename Archive> void serialize(Archive & ar, const unsigned int /*version*/)
        {
            ar & euid_range;
            ar & events;
        }
    };

    /// This struct holds an EventsRange as a char buffer. This is used in
    /// the pipeline for zlib and boost::serialization.
    struct EventsRangeAsBytes
    {
        EuidRange euid_range;
        std::vector<char> event_bytes;
    };

    /// Base class for event "snooper". Receives EventAccessors for
    /// every cosim step().
    class CoSimPipelineSnooper
    {
      public:
        virtual ~CoSimPipelineSnooper() = default;
        virtual void onNewEvent(EventAccessor && evt) = 0;
    };

    /// This class implements a performant pipeline to process
    /// cosim events. A series of transformations will occur on
    /// the events on a background thread on their way to the DB,
    /// and the pipeline has an API to access events via the
    /// EventAccessor class. This allows you to get access to
    /// an event which may live in the pipeline cache or on disk.
    ///
    class CoSimPipeline : public simdb::App
    {
      public:
        /// All SimDB apps must provide a NAME
        static constexpr auto NAME = "cosim-pipeline";

        /// Constructor must have this signature for simdb::AppManager factories
        CoSimPipeline(simdb::DatabaseManager* db_mgr) : db_mgr_(db_mgr) {}

        ~CoSimPipeline() noexcept = default;

        /// Define the SimDB schema.
        static void defineSchema(simdb::Schema & schema);

        /// Tell us how many internal pipelines to create (one per hart).
        void setNumHarts(size_t num_harts);

        /// Returns a pipeline configured for fast event processing and retrieval.
        std::unique_ptr<simdb::pipeline::Pipeline>
        createPipeline(simdb::pipeline::AsyncDatabaseAccessor* db_accessor) override;

        /// Called by unit tests to validate async event retrieval.
        void setSnooper(CoSimPipelineSnooper* snooper);

        /// Called by CoSimObserver during postExecute().
        void process(Event && evt);

        /// Get the event uid for the most recent event for this hart.
        uint64_t getLastEventUID(HartId hart_id = 0) const;

        /// Get an EventAccessor for the most recent event for this hart.
        EventAccessor getLastEvent(HartId hart_id = 0);

        /// Create an EventAccessor for the given event uid and hart.
        EventAccessor getEvent(uint64_t euid, HartId hart_id = 0);

        /// Called by the CoSimObserver when Pegasus calls stopSim().
        void stopSim(HartId hart_id = 0);

        /// Check if the given hart has reached its stopSim().
        bool simStopped(HartId hart_id = 0) const;

        /// Called after the pipeline threads have been flushed and destroyed.
        void postTeardown() override;

      private:
        friend class EventAccessor;

        /// Return a pointer to the event in the cache. Not safe to use
        /// this pointer later as it may have been removed from the cache.
        const Event* getEventFromCache_(uint64_t euid, HartId hart_id = 0) const;

        /// If an event is not found in the cache, recreate it from disk.
        std::unique_ptr<Event> recreateEventFromDisk_(uint64_t euid, HartId hart_id = 0);

        /// One pipeline impl per hart (their own cache, their own mutex, ...)
        class CoSimHartPipeline
        {
          public:
            CoSimHartPipeline(simdb::pipeline::AsyncDatabaseAccessor* async_eval, HartId hart_id) :
                async_eval_(async_eval),
                hart_id_(hart_id)
            {
            }

            ~CoSimHartPipeline() = default;

            /// Add a copy of the given event to the cache.
            void addToCache(const Event & evt);

            /// Get the uid of the last cached event.
            uint64_t getLastEventUID() const;

            /// Get an event by its uid from the cache. Note that
            /// even if this returns a valid event, you should not
            /// cache it for later use as it may have been evicted
            /// from the cache and destroyed.
            const Event* getEventFromCache(uint64_t euid) const;

            /// Recreate an old event from disk when it is no longer
            /// available in the cache.
            std::unique_ptr<Event> recreateEventFromDisk(uint64_t euid);

            /// Evict a range of events from the cache.
            void evict(const EuidRange & euid_range);

            /// Set the most recent event's "ends simulation / done" flags.
            void stopSim(StopEvent & stop_event);

            /// Called after pipeline threads have been closed.
            /// Prints metrics about cache/disk retrieval to stdout.
            void postTeardown();

          private:
            /// Async DB query object.
            simdb::pipeline::AsyncDatabaseAccessor* async_eval_ = nullptr;

            /// Hart ID.
            HartId hart_id_;

            /// Mutex to protect the cache and last event uid.
            mutable std::mutex mutex_;

            /// Event cache. A deque is used so we can efficiently
            /// grow the cache without bounds on one side, and only
            /// remove them from the other side when the pipeline
            /// sends us eviction notices on ranges of events.
            std::deque<Event> evt_cache_;

            /// Last seen event uid.
            sparta::utils::ValidValue<uint64_t> last_event_uid_;

            /// Metrics to print out usage and performance to stdout.
            mutable size_t num_evts_retrieved_from_cache_ = 0;
            simdb::RunningMean avg_microseconds_recreating_evts_;

            friend class CoSimPipeline;
        };

        /// SimDB instance.
        simdb::DatabaseManager* db_mgr_ = nullptr;

        /// One pipeline impl per hart.
        std::vector<std::unique_ptr<CoSimHartPipeline>> hart_pipelines_;

        /// One pipeline input queue per hart.
        std::vector<std::unique_ptr<simdb::ConcurrentQueue<Event>>> pipeline_input_queues_;

        /// One "sim stopped" flag per hart.
        std::vector<bool> sim_stopped_;

        /// Snooper which inspects events as they come through the pipeline.
        CoSimPipelineSnooper* snooper_ = nullptr;
    };

} // namespace pegasus::cosim
