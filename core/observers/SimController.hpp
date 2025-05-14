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
        void preExecute_(AtlasState*) override;
        void postExecute_(AtlasState*) override;
        void preException_(AtlasState*) override;

        class SimEndpoint;
        std::shared_ptr<SimEndpoint> endpoint_;
    };

} // namespace atlas
