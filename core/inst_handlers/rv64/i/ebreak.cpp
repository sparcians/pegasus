#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::ebreak_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // if (!STATE.debug_mode && (
        //         (!STATE.v && STATE.prv == PRV_M && STATE.dcsr->ebreakm) ||
        //         (!STATE.v && STATE.prv == PRV_S && STATE.dcsr->ebreaks) ||
        //         (!STATE.v && STATE.prv == PRV_U && STATE.dcsr->ebreaku) ||
        //         (STATE.v && STATE.prv == PRV_S && STATE.dcsr->ebreakvs) ||
        //         (STATE.v && STATE.prv == PRV_U && STATE.dcsr->ebreakvu))) {
        //     throw trap_debug_mode();
        // } else {
        //     throw trap_breakpoint(STATE.v, pc);
        // }

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        return nullptr;
    }
} // namespace atlas
