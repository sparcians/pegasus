#include "test/sim/InstructionTester.hpp"
#include "core/VecElements.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "mavis/Mavis.h"

class VmInstructionTester : public AtlasInstructionTester
{

  public:
    using VLEN = std::array<uint8_t, 8>;
    using XLEN = uint64_t;

    VmInstructionTester() = default;

    void testVmandmm()
    {
        atlas::AtlasState* state = getAtlasState();
        size_t vstart = 1 * 8;
        size_t vl = 4 * 8;

        state->getVectorConfig()->setVLEN(64);
        state->getVectorConfig()->setVSTART(vstart);
        state->getVectorConfig()->setVL(vl);

        VLEN vs1_val = {255, 255, 0, 255, 255, 255, 255, 0};
        VLEN vs2_val = {0, 0, 255, 255, 0, 0, 255, 255};
        VLEN v3_val = {255, 0, 0, 255, 255, 255, 0, 255};

        WRITE_VEC_REG<VLEN>(state, 1, vs1_val);
        WRITE_VEC_REG<VLEN>(state, 2, vs2_val);
        WRITE_VEC_REG<VLEN>(state, 3, v3_val);
        instPtr_ = makeAtlasInst(std::string{"vmand.mm"}, mavis::ExtractorIF::RegListType{1, 2},
                                 mavis::ExtractorIF::RegListType{3}); // vd = 3, vs1 = 1, vs2 = 2
        executeInstruction(instPtr_);

        auto vd_val = READ_VEC_REG<VLEN>(state, 3);
        EXPECT_EQUAL(vd_val[0], v3_val[0]); // head
        for (size_t i = 1; i < 4; ++i)      // body
        {
            EXPECT_EQUAL(vd_val[i], vs1_val[i] & vs2_val[i]);
        }
        for (size_t i = 4; i < sizeof(vd_val); ++i) // tail
        {
            EXPECT_EQUAL(vd_val[i], v3_val[i]);
        }
        const atlas::AtlasState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    void testVpopcm()
    {
        atlas::AtlasState* state = getAtlasState();
        const atlas::Addr pc = 0x1000;
        uint32_t opcode;
        size_t vstart = 1 * 8;
        size_t vl = 6 * 8;
        uint32_t rd = 1, vs2 = 2;

        state->getVectorConfig()->setVLEN(64);
        state->getVectorConfig()->setVSTART(vstart);
        state->getVectorConfig()->setVL(vl);

        VLEN vs2_val = {1, 0, 1, 0, 1, 0, 1, 0};
        VLEN v0_val = {1, 0, 1, 0, 0, 0, 1, 0};
        WRITE_VEC_REG<VLEN>(state, vs2, vs2_val);
        WRITE_VEC_REG<VLEN>(state, atlas::V0, v0_val);
        opcode = vpopcmOp(rd, vs2, 0); // masked
        injectInstruction(pc, opcode);

        EXPECT_EQUAL(READ_INT_REG<XLEN>(state, rd), 1);

        const atlas::AtlasState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 2);
    }

    void testVfirst()
    {
        atlas::AtlasState* state = getAtlasState();
        const atlas::Addr pc = 0x1000;
        uint32_t opcode;
        size_t vstart = 1 * 8;
        size_t vl = 6 * 8;
        uint32_t rd = 1, vs2 = 2;

        state->getVectorConfig()->setVLEN(64);
        state->getVectorConfig()->setVSTART(vstart);
        state->getVectorConfig()->setVL(vl);

        VLEN vs2_val = {1, 0, 1, 0, 1, 0, 1, 0};
        VLEN v0_val = {1, 0, 0, 0, 1, 0, 1, 0};
        WRITE_VEC_REG<VLEN>(state, vs2, vs2_val);
        WRITE_VEC_REG<VLEN>(state, atlas::V0, v0_val);
        opcode = vfirstOp(rd, vs2, 0); // masked
        injectInstruction(pc, opcode);

        EXPECT_EQUAL(READ_INT_REG<XLEN>(state, rd), 32);

        const atlas::AtlasState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 3);
    }

    uint32_t vpopcmOp(uint8_t rd, uint8_t vs2, uint8_t vm)
    {
        uint32_t opcode = 0x40082057;
        uint8_t offset = 0;
        offset = 7;
        opcode |= rd << offset; // rd
        offset = 20;
        opcode |= vs2 << offset; // vs2
        offset = 25;
        opcode |= vm << offset; // vm
        return opcode;
    }

    uint32_t vfirstOp(uint8_t rd, uint8_t vs2, uint8_t vm)
    {
        uint32_t opcode = 0x4008a057;
        uint8_t offset = 0;
        offset = 7;
        opcode |= rd << offset; // rd
        offset = 20;
        opcode |= vs2 << offset; // vs2
        offset = 25;
        opcode |= vm << offset; // vm
        return opcode;
    }

  private:
    atlas::AtlasInst::PtrType instPtr_ = nullptr;
};

int main()
{
    VmInstructionTester Vm_tester;

    Vm_tester.testVmandmm();
    Vm_tester.testVpopcm();
    Vm_tester.testVfirst();

    REPORT_ERROR;
    return ERROR_CODE;
}
