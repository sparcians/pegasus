#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::dret_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require(STATE.debug_mode);
        // set_pc_and_serialize(STATE.dpc->read());
        // if (ZICFILP_xLPE(STATE.dcsr->v, STATE.dcsr->prv)) {
        //   STATE.elp = STATE.dcsr->pelp;
        // }
        // p->set_privilege(STATE.dcsr->prv, STATE.dcsr->v);
        // if (STATE.prv < PRV_M) {
        //   STATE.mstatus->write(STATE.mstatus->read() & ~MSTATUS_MPRV);
        //   STATE.mstatus->write(STATE.mstatus->read() & ~MSTATUS_MDT);
        // }
        //
        // if (STATE.dcsr->prv == PRV_U || STATE.dcsr->v)
        //   STATE.mstatus->write(STATE.mstatus->read() & ~MSTATUS_SDT);
        //
        // if (STATE.dcsr->v && STATE.dcsr->prv == PRV_U)
        //   STATE.vsstatus->write(STATE.vsstatus->read() & ~SSTATUS_SDT);
        //
        // if (STATE.dcsr->prv == PRV_U || STATE.dcsr->v)
        //   STATE.mstatus->write(STATE.mstatus->read() & ~MSTATUS_SDT);
        //
        // if (STATE.dcsr->v && STATE.dcsr->prv == PRV_U)
        //   STATE.vsstatus->write(STATE.vsstatus->read() & ~SSTATUS_SDT);
        //
        ///* We're not in Debug Mode anymore. */
        // STATE.debug_mode = false;
        //
        // if (STATE.dcsr->step)
        //   STATE.single_step = STATE.STEP_STEPPING;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        return nullptr;
    }
} // namespace atlas
