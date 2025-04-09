#pragma once

#include "core/observers/Observer.hpp"
#include <memory>

namespace atlas
{

    class AtlasState;

    class SimController : public Observer
    {
      public:
        using base_type = SimController;

        SimController();

        void postInit(AtlasState*);
        void onSimulationFinished(AtlasState* state);

      private:
        ActionGroup* preExecute_(AtlasState* state) override;
        ActionGroup* postExecute_(AtlasState* state) override;
        ActionGroup* preException_(AtlasState* state) override;

        class SimEndpoint;
        std::shared_ptr<SimEndpoint> endpoint_;
    };

} // namespace atlas
