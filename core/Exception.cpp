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
}

void Exception::handleSModeException_(atlas::AtlasState* state)
{
    // TODO cnyce
    (void)state;
}

void Exception::handleMModeException_(atlas::AtlasState* state)
{
    const reg_t trap_handler_address = (READ_CSR_REG(MTVEC) & ~(reg_t)1);
    state->setNextPc(trap_handler_address);

    const reg_t epc = state->getPc();
    WRITE_CSR_REG(MEPC, epc);

    const uint64_t cause = static_cast<uint64_t>(cause_.getValue());
    WRITE_CSR_REG(MCAUSE, cause);
    const uint64_t read_cause = READ_CSR_REG(MCAUSE);
    (void)read_cause;

    const uint64_t mtval = state->getCurrentInst()->getOpcode();
    WRITE_CSR_REG(MTVAL, mtval);
    const uint64_t read_mtval = READ_CSR_REG(MTVAL);
    (void)read_mtval;

    const uint64_t mtval2 = 0;
    WRITE_CSR_REG(MTVAL2, mtval2);
    const uint64_t mtval22 = READ_CSR_REG(MTVAL2);
    (void)mtval22;

    const uint64_t mtinst = 0;
    WRITE_CSR_REG(MTINST, mtinst);
    const uint64_t mtinst2 = READ_CSR_REG(MTINST);
    (void)mtinst2;

    // Need MSTATUS initial value. See Spike "compute_mstatus_initial_value".

    uint64_t mstatus = READ_CSR_REG(MSTATUS);
    const auto mstatus_mie = READ_CSR_FIELD(MSTATUS, mie);
    WRITE_CSR_FIELD(MSTATUS, mpie, mstatus_mie);
    mstatus = READ_CSR_REG(MSTATUS);

    const auto mpp = static_cast<uint64_t>(state->getPrivMode());
    WRITE_CSR_FIELD(MSTATUS, mpp, mpp);
    mstatus = READ_CSR_REG(MSTATUS);

    const uint64_t mie = 0;
    WRITE_CSR_FIELD(MSTATUS, mie, mie);
    mstatus = READ_CSR_REG(MSTATUS);

    const uint64_t mpv = 0;
    WRITE_CSR_FIELD(MSTATUS, mpv, mpv);
    mstatus = READ_CSR_REG(MSTATUS);

    const uint64_t gva = 0;
    WRITE_CSR_FIELD(MSTATUS, gva, gva);
    mstatus = READ_CSR_REG(MSTATUS);

    (void)mstatus;
}

} // namespace atlas
