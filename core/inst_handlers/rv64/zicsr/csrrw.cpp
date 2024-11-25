#include "core/inst_handlers/rv64/zicsr/RvzicsrInsts.hpp"

// #include "core/ActionGroup.hpp"
// #include "core/AtlasState.hpp"
// #include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvzicsrInsts::csrrw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // int csr = validate_csr(insn.csr(), true);
        // reg_t old = p->get_csr(csr, insn, true);
        // p->put_csr(csr, RS1);
        // WRITE_RD(sext_xlen(old));
        // serialize();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas