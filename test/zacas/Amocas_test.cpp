#include "test/sim/InstructionTester.hpp"
#include "core/VecElements.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "mavis/Mavis.h"

class AmocasInstructionTester : public PegasusInstructionTester
{

  public:
    using XLEN = uint32_t;
    using WORD = uint32_t;
    using DOUBLE = uint64_t;

    AmocasInstructionTester() : PegasusInstructionTester("rv32imafdcbv_zicsr_zifencei_zacas") {}

    void test_amocas_w()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        const uint32_t rs1_val = 0x2000;
        const WORD temp = 0x80000000;
        const uint32_t rs2_val = 0xFFFFFFFF;
        const uint32_t rd_val = 0x80000000;
        // mavis requires source and destination registers to be even numbered.
        const uint32_t rd = 6, rs1 = 2, rs2 = 4;
        uint32_t opcode;

        WRITE_INT_REG<XLEN>(state, rs1, rs1_val);
        WRITE_INT_REG<XLEN>(state, rs2, rs2_val);
        WRITE_INT_REG<XLEN>(state, rd, rd_val);
        state->writeMemory<WORD>(rs1_val, temp);
        opcode = genAmocasOpcode(rd, rs1, rs2);

        injectInstruction(pc, opcode);

        const XLEN rd_v = READ_INT_REG<XLEN>(state, rd);
        EXPECT_EQUAL(rd_v, 0x80000000);
        const WORD mem_v = state->readMemory<WORD>(rs1_val);
        EXPECT_EQUAL(mem_v, rs2_val); // Value swapped

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    uint32_t genAmocasOpcode(uint8_t rd, uint8_t rs1, uint8_t rs2)
    {
        uint32_t stencil = 0x2800302f;
        stencil |= rd << 7;
        stencil |= rs1 << 15;
        stencil |= rs2 << 20;
        return stencil;
    }

  private:
    pegasus::PegasusInst::PtrType instPtr_ = nullptr;
};

int main()
{
    AmocasInstructionTester Amocas_Tester;

    Amocas_Tester.test_amocas_w();

    REPORT_ERROR;
    return ERROR_CODE;
}
