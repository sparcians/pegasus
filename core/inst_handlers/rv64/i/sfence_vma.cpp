#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::sfence_vma_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('S');
        // require_impl(IMPL_MMU);
        // if (STATE.v) {
        //   if (STATE.prv == PRV_U || get_field(STATE.hstatus->read(), HSTATUS_VTVM))
        //     require_novirt();
        // } else {
        //   require_privilege(get_field(STATE.mstatus->read(), MSTATUS_TVM) ? PRV_M :
        //   PRV_S);
        // }
        // MMU.flush_tlb();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        return nullptr;
    }
} // namespace atlas
