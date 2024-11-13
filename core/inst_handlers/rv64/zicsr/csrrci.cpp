#include "core/inst_handlers/rv64/zicsr/RvzicsrInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{

    ActionGroup* RvzicsrInsts::csrrci_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // bool write = insn.rs1() != 0;
        // int csr = validate_csr(insn.csr(), write);
        // reg_t old = p->get_csr(csr, insn, write);
        // if (write) {
        //     p->put_csr(csr, old & ~(reg_t)insn.rs1());
        // }
        // WRITE_RD(sext_xlen(old));
        // serialize();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas