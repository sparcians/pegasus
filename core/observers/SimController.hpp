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
        Action* preExecute_(AtlasState*, Action*) override;
        Action* postExecute_(AtlasState*, Action*) override;
        Action* preException_(AtlasState*, Action*) override;

        class SimEndpoint;
        std::shared_ptr<SimEndpoint> endpoint_;
    };

} // namespace atlas
