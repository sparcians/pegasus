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
    ActionGroup* preExecute(AtlasState* state) override;
    ActionGroup* postExecute(AtlasState* state) override;
    ActionGroup* preException(AtlasState* state) override;

private:
    class SimEndpoint;
    std::shared_ptr<SimEndpoint> endpoint_;
};

} // namespace atlas
