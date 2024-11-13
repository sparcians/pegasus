#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::ecall_64_handler(atlas::AtlasState* state)
    {
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // switch (STATE.prv)
        //{
        //   case PRV_U: throw trap_user_ecall();
        //   case PRV_S:
        //     if (STATE.v)
        //       throw trap_virtual_supervisor_ecall();
        //     else
        //       throw trap_supervisor_ecall();
        //   case PRV_M: throw trap_machine_ecall();
        //   default: abort();
        // }

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        // Command
        const uint64_t cmd = state->getAtlasRegister(RegType::INTEGER, 17)->read(); // a7

        // Only support exit for now so we can end simulation
        if (cmd == 93)
        {
            // Function arguments are a0-a6 (x10-x16)
            const uint64_t exit_code = state->getAtlasRegister(RegType::INTEGER, 10)->read();

            AtlasState::SimState* sim_state = state->getSimState();
            sim_state->workload_exit_code = exit_code;
            sim_state->test_passed = (exit_code == 0) ? true : false;
            sim_state->sim_stopped = true;

            // Stop simulation
            return state->getStopSimActionGroup();
        }

        return nullptr;
    }
} // namespace atlas
