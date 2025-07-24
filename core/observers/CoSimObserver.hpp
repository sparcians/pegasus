#pragma once

#include "core/observers/Observer.hpp"
#include "cosim/Event.hpp"
#include "sparta/utils/SpartaAssert.hpp"
#include "sparta/utils/LogUtils.hpp"

#ifndef COSIMLOG
#define COSIMLOG(msg) SPARTA_LOG(cosim_logger_, msg)
#endif

namespace pegasus::cosim
{
    class CoSimPipeline;

    class CoSimObserver : public Observer
    {
      public:
        using base_type = CoSimObserver;

        CoSimObserver(sparta::log::MessageSource & cosim_logger, CoSimPipeline* cosim_pipeline,
                      HartId hart_id);

      private:
        void preExecute_(PegasusState*) override;
        void postExecute_(PegasusState*) override;
        void preException_(PegasusState*) override;
        void resetLastEvent_(PegasusState* state);
        void sendLastEvent_();
        void stopSim() override;

        sparta::log::MessageSource & cosim_logger_;
        CoSimPipeline* cosim_pipeline_ = nullptr;
        HartId hart_id_;
        uint64_t event_uid_ = 0;
        sparta::utils::ValidValue<Event> last_event_;
    };
} // namespace pegasus::cosim
