#include "test/sim/InstructionTester.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "core/VectorState.hpp"

class ViaInstructionTester : public AtlasInstructionTester
{

  public:
    ViaInstructionTester() = default;

    void testVaddvv1()
    {
        using VLEN = std::array<uint8_t, 8>;
        using XLEN = uint64_t;
        uint64_t pc;
        uint32_t opcode;
        uint32_t rd = 1;
        atlas::AtlasState* state = getAtlasState();
        state->getVectorState()->setVSTART(0);

        pc = 0x1000;
        opcode = vsetivliOp(rd, 8, 0, 0, 0, 0); // vl = 8, vlmul = 1, sew = 8
        injectInstruction(pc, opcode);
        EXPECT_EQUAL(READ_INT_REG<XLEN>(state, rd), 8);

        VLEN vs1_val = {0, 1, 2, 3, 4, 5, 6, 7};
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};
        VLEN vs3_val = {1, 3, 5, 7, 9, 11, 13, 15};
        WRITE_VEC_REG<VLEN>(state, 1, vs1_val);
        WRITE_VEC_REG<VLEN>(state, 2, vs2_val);
        opcode = vaddvvOp(3, 2, 1); // vd = 3, vs1 = 2, vs2 = 1
        injectInstruction(pc, opcode);

        auto vd_val = READ_VEC_REG<VLEN>(state, 3);
        for (size_t i = 0; i < vd_val.size(); ++i)
        {
            EXPECT_EQUAL(vd_val[i], vs3_val[i]);
        }
        const atlas::AtlasState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 2);
    }

    void testVaddvv2()
    {
        using VLEN = std::array<uint8_t, 8>;
        using XLEN = uint64_t;
        uint64_t pc;
        uint32_t opcode;
        uint32_t rd = 1;
        atlas::AtlasState* state = getAtlasState();
        state->getVectorState()->setVSTART(0);

        pc = 0x1000;
        opcode = vsetivliOp(rd, 16, 1, 0, 0, 0); // vl = 16, vlmul = 2, sew = 8
        injectInstruction(pc, opcode);
        EXPECT_EQUAL(READ_INT_REG<XLEN>(state, rd), 16);

        VLEN vs1_val = {0, 1, 2, 3, 4, 5, 6, 7};
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};
        VLEN vs3_val = {9, 10, 11, 12, 13, 14, 15, 16};
        VLEN vs4_val = {17, 18, 19, 20, 21, 22, 23, 24};
        VLEN vs5_val = {9, 11, 13, 15, 17, 19, 21, 23};
        VLEN vs6_val = {18, 20, 22, 24, 26, 28, 30, 32};
        WRITE_VEC_REG<VLEN>(state, 1, vs1_val);
        WRITE_VEC_REG<VLEN>(state, 2, vs2_val);
        WRITE_VEC_REG<VLEN>(state, 3, vs3_val);
        WRITE_VEC_REG<VLEN>(state, 4, vs4_val);
        opcode = vaddvvOp(5, 3, 1); // vd = 5, vs1 = 3, vs2 = 1
        injectInstruction(pc, opcode);

        auto vd_val1 = READ_VEC_REG<VLEN>(state, 5);
        for (size_t i = 0; i < vd_val1.size(); ++i)
        {
            EXPECT_EQUAL(vd_val1[i], vs5_val[i]);
        }
        auto vd_val2 = READ_VEC_REG<VLEN>(state, 6);
        for (size_t i = 0; i < vd_val2.size(); ++i)
        {
            EXPECT_EQUAL(vd_val2[i], vs6_val[i]);
        }
        const atlas::AtlasState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 4);
    }

  private:
    uint32_t vsetivliOp(uint8_t rd, uint8_t avl, uint8_t vlmul, uint8_t sew, uint8_t vta,
                        uint8_t vma)
    {
        uint32_t opcode = 0;
        uint8_t offset = 0;
        opcode |= 0x57 << offset;
        offset += 7;
        opcode |= rd << offset;
        offset += 5;
        opcode |= 7 << offset;
        offset += 3;
        opcode |= avl << offset;
        offset += 5;
        opcode |= vlmul << offset;
        offset += 3;
        opcode |= sew << offset;
        offset += 3;
        opcode |= vta << offset;
        offset += 1;
        opcode |= vma << offset;
        offset += 1;
        offset += 2;
        opcode |= 1 << offset;
        offset += 1;
        opcode |= 1 << offset;
        offset += 1;
        assert(offset == 32);
        return opcode;
    }

    uint32_t vaddvvOp(uint8_t rd, uint8_t rs1, uint8_t rs2)
    {
        uint32_t opcode = 0x57;
        opcode |= rd << 7;
        opcode |= rs1 << 15;
        opcode |= rs2 << 20;
        return opcode;
    }
};

int main()
{
    ViaInstructionTester Via_tester;

    Via_tester.testVaddvv1();
    Via_tester.testVaddvv2();

    REPORT_ERROR;
    return ERROR_CODE;
}
