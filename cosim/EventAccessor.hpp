#pragma once

#include "include/PegasusTypes.hpp"

/// Events go into the pipeline one at a time, and are buffered for
/// performance reasons (more zlib compression, fewer DB writes).
#define DEFAULT_EVENT_WINDOW_SIZE 100

namespace pegasus::cosim
{

    class Event;
    class CoSimPipeline;

    /// The EventAccessor class is used as a proxy to access
    /// events after step(). The reason a proxy is used is that
    /// the event's location in the pipeline changes in a manner
    /// that is unpredictable to the cosim client.
    class EventAccessor
    {
      public:
        EventAccessor(uint64_t euid, HartId hart_id, CoSimPipeline* cosim_pipeline) :
            euid_(euid),
            hart_id_(hart_id),
            cosim_pipeline_(cosim_pipeline)
        {
        }

        EventAccessor() = default;
        EventAccessor(const EventAccessor & other) = default;
        EventAccessor(EventAccessor && other) = default;
        EventAccessor & operator=(const EventAccessor & other) = default;
        EventAccessor & operator=(EventAccessor && other) = default;

        uint64_t getEuid() const { return euid_; }

        /// It is strongly recommended that you call operator->()
        /// for EVERY api call you want to make. The event could
        /// be valid one second and deallocated the next.
        ///
        /// Do this:
        ///   auto done = accessor->isDone();              // maybe got from cache
        ///   auto type = accessor->getEventType();        // maybe had to go to disk
        ///
        /// Not this:
        ///   Event* evt = accessor.operator->();
        ///   auto done = evt->isDone();                   // could be alive
        ///   auto type = evt->getEventType();             // could be dead!
        const Event* operator->();

        /// Same as operator->().
        const Event* get();

        /// How many times did we have to go to disk to get this event?
        size_t getNumFromDisk() const { return num_from_disk_; }

        /// How many times did we get this event from the cache?
        size_t getNumFromCache() const { return num_from_cache_; }

      private:
        uint64_t euid_;
        HartId hart_id_;
        CoSimPipeline* cosim_pipeline_ = nullptr;
        std::shared_ptr<Event> recreated_evt_;
        size_t num_from_disk_ = 0;
        size_t num_from_cache_ = 0;
    };

} // namespace pegasus::cosim
