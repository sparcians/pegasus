#pragma once

#include "include/PegasusTypes.hpp"

#include <cinttypes>
#include <memory>

namespace pegasus::cosim
{

    class Event;
    class CoSimEventPipeline;

    /// The EventAccessor class is used as a proxy to access
    /// events after step(). The reason a proxy is used is that
    /// the event's location in the pipeline changes in a manner
    /// that is unpredictable to the cosim client.
    class EventAccessor
    {
      public:
        EventAccessor(uint64_t euid, CoreId core_id, HartId hart_id,
                      CoSimEventPipeline* evt_pipeline) :
            euid_(euid),
            core_id_(core_id),
            hart_id_(hart_id),
            evt_pipeline_(evt_pipeline)
        {
        }

        EventAccessor() = default;
        EventAccessor(const EventAccessor & other) = default;
        EventAccessor(EventAccessor && other) = default;
        EventAccessor & operator=(const EventAccessor & other) = default;
        EventAccessor & operator=(EventAccessor && other) = default;

        uint64_t getEuid() const { return euid_; }

        CoreId getCoreId() const { return core_id_; }

        HartId getHartId() const { return hart_id_; }

        /// It is strongly recommended that you call operator->()
        /// for EVERY api call you want to make. The event could
        /// be valid one second and deallocated the next.
        ///
        /// Do this:
        ///   auto done = accessor->isDone();              // maybe got from cache
        ///   auto type = accessor->getEventType();        // maybe had to go to disk
        ///
        /// Not this:
        ///   Event* evt = accessor.get();
        ///   auto done = evt->isDone();                   // could be alive
        ///   auto type = evt->getEventType();             // could be dead!
        const Event* operator->();

        /// Same as operator->().
        const Event* get(bool must_exist = true);

        /// How many times did we have to go to disk to get this event?
        size_t getNumFromDisk() const { return num_from_disk_; }

        /// How many times did we get this event from the cache?
        size_t getNumFromCache() const { return num_from_cache_; }

        /// How many times did we have to recreate this event from the pipeline?
        size_t getNumFromPipeline() const { return num_from_pipeline_; }

      private:
        uint64_t euid_;
        CoreId core_id_;
        HartId hart_id_;
        CoSimEventPipeline* evt_pipeline_ = nullptr;
        std::shared_ptr<Event> recreated_evt_;
        size_t num_from_disk_ = 0;
        size_t num_from_cache_ = 0;
        size_t num_from_pipeline_ = 0;
    };

} // namespace pegasus::cosim
