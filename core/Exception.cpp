#include "core/Exception.hpp"
#include "core/Fetch.hpp"
#include "core/AtlasState.hpp"
#include "include/ActionTags.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"
#include "include/CSRFieldIdxs64.hpp"
#include "core/inst_handlers/inst_helpers.hpp"

namespace atlas
{

    Exception::Exception(sparta::TreeNode* exception_node, const ExceptionParameters*) :
        sparta::Unit(exception_node)
    {
        Action exception_action = atlas::Action::createAction<&Exception::handleException_>(
            this, "exception", ActionTags::EXCEPTION_TAG);
        exception_action_group_.addAction(exception_action);
    }

    void Exception::onBindTreeEarly_()
    {
        auto core_tn = getContainer()->getParentAs<sparta::ResourceTreeNode>();
        AtlasState* state = core_tn->getResourceAs<AtlasState>();

        // Connect exception ActionGroup to instruction finish ActionGroup
        exception_action_group_.setNextActionGroup(state->getFinishActionGroup());
    }

    ActionGroup* Exception::handleException_(atlas::AtlasState* state)
    {
        sparta_assert(cause_.isValid(), "Exception cause is not valid!");

        switch (state->getPrivMode())
        {
            case PrivMode::USER:
                handleUModeException_<RV64>(state);
                break;
            case PrivMode::MACHINE:
                handleMModeException_<RV64>(state);
                break;
            case PrivMode::SUPERVISOR:
                handleSModeException_<RV64>(state);
                break;
            default:
                sparta_assert(false, "Illegal privilege mode");
        }

        state->snapshotAndSyncWithCoSim();
        cause_.clearValid();
        return nullptr;
    }

    template <typename XLEN> void Exception::handleUModeException_(atlas::AtlasState* state)
    {
        (void)state;
        sparta_assert(false, "Not implemented");
    }

    template <typename XLEN> void Exception::handleSModeException_(atlas::AtlasState* state)
    {
        (void)state;
        sparta_assert(false, "Not implemented");
    }

    template <typename XLEN> void Exception::handleMModeException_(atlas::AtlasState* state)
    {
        const reg_t trap_handler_address = (READ_CSR_REG<XLEN>(state, MTVEC) & ~(reg_t)1);
        state->setNextPc(trap_handler_address);

        const reg_t epc = state->getPc();
        WRITE_CSR_REG<XLEN>(state, MEPC, epc);

        const uint64_t cause = static_cast<uint64_t>(cause_.getValue());
        WRITE_CSR_REG<XLEN>(state, MCAUSE, cause);

        const uint64_t mtval = state->getSimState()->current_opcode;
        WRITE_CSR_REG<XLEN>(state, MTVAL, mtval);

        const uint64_t mtval2 = 0;
        WRITE_CSR_REG<XLEN>(state, MTVAL2, mtval2);

        const uint64_t mtinst = 0;
        WRITE_CSR_REG<XLEN>(state, MTINST, mtinst);

        // Need MSTATUS initial value. See "compute_mstatus_initial_value".

        const auto mstatus_mie = READ_CSR_FIELD<XLEN>(state, MSTATUS, "mie");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpie", mstatus_mie);

        const auto mpp = static_cast<uint64_t>(state->getPrivMode());
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpp", mpp);

        const uint64_t mie = 0;
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mie", mie);

        const uint64_t mpv = 0;
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpv", mpv);

        const uint64_t gva = 0;
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "gva", gva);
    }

} // namespace atlas
