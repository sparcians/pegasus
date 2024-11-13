#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::wfi_64_handler(atlas::AtlasState* state)
    {
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // if (get_field(STATE.mstatus->read(), MSTATUS_TW)) {
        //   require_privilege(PRV_M);
        // } else if (STATE.v) {
        //   if (STATE.prv == PRV_U || get_field(STATE.hstatus->read(), HSTATUS_VTW))
        //     require_novirt();
        // } else if (p->extension_enabled('S')) {
        //   // When S-mode is implemented, then executing WFI in
        //   // U-mode causes an illegal instruction exception.
        //   require_privilege(PRV_S);
        // }
        // wfi();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        if (state->getStopSimOnWfi())
        {
            AtlasState::SimState* sim_state = state->getSimState();
            sim_state->sim_stopped = true;
            return state->getStopSimActionGroup();
        }
        return nullptr;
    }
} // namespace atlas
