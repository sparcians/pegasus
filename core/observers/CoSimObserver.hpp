#pragma once

#include "core/observers/Observer.hpp"
#include "cosim/Event.hpp"
#include "sparta/utils/SpartaAssert.hpp"
#include "sparta/utils/LogUtils.hpp"

#ifndef COSIMLOG
#define COSIMLOG(msg) SPARTA_LOG(cosim_logger_, msg)
#endif

namespace sparta::serialization::checkpoint
{
    class CherryPickFastCheckpointer;
}

using CoSimCheckpointer = sparta::serialization::checkpoint::CherryPickFastCheckpointer;

namespace pegasus
{
    class PegasusState;
}

namespace pegasus::cosim
{
    class CoSimEventPipeline;

    class CoSimObserver : public Observer
    {
      public:
        using base_type = CoSimObserver;

        CoSimObserver(sparta::log::MessageSource & cosim_logger, CoSimEventPipeline* evt_pipeline,
                      CoSimCheckpointer* checkpointer, CoreId core_id, HartId hart_id);

        CoSimEventPipeline* getEventPipeline();
        const CoSimEventPipeline* getEventPipeline() const;

        CoSimCheckpointer* getCheckpointer();
        const CoSimCheckpointer* getCheckpointer() const;

      private:
        void preExecute_(PegasusState*) override;
        void postExecute_(PegasusState*) override;
        void preException_(PegasusState*) override;
        void resetLastEvent_(PegasusState* state);
        void sendLastEvent_();
        void stopSim() override;

        sparta::log::MessageSource & cosim_logger_;
        CoSimEventPipeline* evt_pipeline_ = nullptr;
        CoSimCheckpointer* checkpointer_ = nullptr;
        const CoreId core_id_;
        const HartId hart_id_;
        sparta::utils::ValidValue<Event> last_event_;

        // Friend needed to access last_event_
        friend class CoSimEventPipeline;
    };
} // namespace pegasus::cosim
