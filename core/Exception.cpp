#include "core/Exception.hpp"
#include "core/Fetch.hpp"
#include "core/AtlasState.hpp"
#include "include/ActionTags.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"
#include "include/CSRFieldIdxs64.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "arch/register_macros.hpp"

namespace atlas
{

Exception::Exception(sparta::TreeNode* exception_node, const ExceptionParameters*)
    : sparta::Unit(exception_node)
{
    post_inst_handler_action_ = atlas::Action::createAction<&Exception::postInstHandler_>(this, "handle exception");
}

void Exception::onBindTreeEarly_()
{
    auto core_tn = getContainer()->getParentAs<sparta::ResourceTreeNode>();
    state_ = core_tn->getResourceAs<AtlasState>();
}

ActionGroup* Exception::postInstHandler_(atlas::AtlasState* state)
{
    if (cause_.isValid()) {
        switch (state->getPrivMode()) {
        case PrivMode::USER:
            handleUModeException_(state);
            break;
        case PrivMode::MACHINE:
            handleMModeException_(state);
            break;
        case PrivMode::SUPERVISOR:
            handleSModeException_(state);
            break;
        default:
            sparta_assert(false, "Illegal privilege mode");
        }
    }

    state->snapshotAndSyncWithCoSim();
    cause_.clearValid();
    return nullptr;
}

void Exception::handleUModeException_(atlas::AtlasState* state)
{
    // TODO cnyce
    (void)state;
    sparta_assert(false, "Not implemented");
}

void Exception::handleSModeException_(atlas::AtlasState* state)
{
    // TODO cnyce
    (void)state;
    sparta_assert(false, "Not implemented");
}

void Exception::handleMModeException_(atlas::AtlasState* state)
{
    const reg_t trap_handler_address = (READ_CSR_REG(MTVEC) & ~(reg_t)1);
    state->setNextPc(trap_handler_address);

    const reg_t epc = state->getPc();
    WRITE_CSR_REG(MEPC, epc);

    const uint64_t cause = static_cast<uint64_t>(cause_.getValue());
    WRITE_CSR_REG(MCAUSE, cause);

    const uint64_t mtval = state->getCurrentInst()->getOpcode();
    WRITE_CSR_REG(MTVAL, mtval);

    const uint64_t mtval2 = 0;
    WRITE_CSR_REG(MTVAL2, mtval2);

    const uint64_t mtinst = 0;
    WRITE_CSR_REG(MTINST, mtinst);

    // Need MSTATUS initial value. See "compute_mstatus_initial_value".

    const auto mstatus_mie = READ_CSR_FIELD(MSTATUS, mie);
    WRITE_CSR_FIELD(MSTATUS, mpie, mstatus_mie);

    const auto mpp = static_cast<uint64_t>(state->getPrivMode());
    WRITE_CSR_FIELD(MSTATUS, mpp, mpp);

    const uint64_t mie = 0;
    WRITE_CSR_FIELD(MSTATUS, mie, mie);

    const uint64_t mpv = 0;
    WRITE_CSR_FIELD(MSTATUS, mpv, mpv);

    const uint64_t gva = 0;
    WRITE_CSR_FIELD(MSTATUS, gva, gva);
}

} // namespace atlas
