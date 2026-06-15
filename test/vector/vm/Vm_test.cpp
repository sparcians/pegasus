#include "test/sim/InstructionTester.hpp"
#include "core/VecElements.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "mavis/Mavis.h"

class VmInstructionTester : public PegasusInstructionTester
{

  public:
    using VLEN = std::array<uint8_t, 8>;
    using XLEN = uint64_t;

    VmInstructionTester() = default;

    void testVmandmm()
    {
        pegasus::PegasusState* state = getPegasusState();
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
        instPtr_ = makePegasusInst(std::string{"vmand.mm"}, mavis::ExtractorIF::RegListType{1, 2},
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
        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    void testVpopcm()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
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
        WRITE_VEC_REG<VLEN>(state, pegasus::V0, v0_val);
        opcode = vpopcmOp(rd, vs2, 0); // masked
        injectInstruction(pc, opcode);

        EXPECT_EQUAL(READ_INT_REG<XLEN>(state, rd), 1);

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 2);
    }

    void testVfirst()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
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
        WRITE_VEC_REG<VLEN>(state, pegasus::V0, v0_val);
        opcode = vfirstOp(rd, vs2, 0); // masked
        injectInstruction(pc, opcode);

        EXPECT_EQUAL(READ_INT_REG<XLEN>(state, rd), 32);

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 3);
    }

    void testVmsbfm()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        uint32_t opcode;
        size_t vstart = 1 * 8;
        size_t vl = 6 * 8;

        state->getVectorConfig()->setVLEN(64);
        state->getVectorConfig()->setVSTART(vstart);
        state->getVectorConfig()->setVL(vl);

        VLEN v2_val = {1, 0, 1, 0, 1, 1, 1, 0};
        VLEN v0_val = {1, 0, 0, 255, 1, 0, 1, 0};
        VLEN v1_val = {0, 0, 0, 0, 0, 0, 0, 0};
        const VLEN vd_val = {0, 0, 0, 255, 0, 0, 0, 0};
        WRITE_VEC_REG<VLEN>(state, pegasus::V2, v2_val);
        WRITE_VEC_REG<VLEN>(state, pegasus::V0, v0_val);
        WRITE_VEC_REG<VLEN>(state, pegasus::V1, v1_val);
        opcode = vmsbfmOp(pegasus::V1, pegasus::V2, 0); // masked
        injectInstruction(pc, opcode);

        v1_val = READ_VEC_REG<VLEN>(state, pegasus::V1);
        for (size_t i = 0; i < 8; ++i)
        {
            EXPECT_EQUAL(v1_val[i], vd_val[i]);
        }

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 4);
    }

    void testViotam()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        const size_t vstart = 0;
        const size_t vl = 8;
        uint32_t opcode = 0;

        state->getVectorConfig()->setVLEN(64);
        state->getVectorConfig()->setVSTART(vstart);
        state->getVectorConfig()->setVL(vl);

        const VLEN v0_val = {1, 1, 0, 1, 0, 1, 1, 1};
        const VLEN v2_val = {1, 0, 0, 0, 1, 0, 0, 1};
        const VLEN v4_val = {0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf};
        WRITE_VEC_REG<VLEN>(state, pegasus::V0, v0_val);
        WRITE_VEC_REG<VLEN>(state, pegasus::V2, v2_val);
        WRITE_VEC_REG<VLEN>(state, pegasus::V4, v4_val);

        opcode = viotamOp(pegasus::V4, pegasus::V2, 0); // unmasked
        injectInstruction(pc, opcode);

        const VLEN vd_val_unmasked_result = {0, 1, 1, 1, 1, 2, 2, 2};
        VLEN vd_val = READ_VEC_REG<VLEN>(state, pegasus::V4);
        for (size_t i = 0; i < 8; ++i)
        {
            EXPECT_EQUAL(vd_val[i], vd_val_unmasked_result[i]);
        }

        // Mask test
        WRITE_VEC_REG<VLEN>(state, pegasus::V4, v4_val);
        const VLEN vd_val_masked_result = {0, 1, 0xf, 1, 0xf, 1, 1, 1};
        opcode = viotamOp(pegasus::V4, pegasus::V2, 1); // masked
        injectInstruction(pc, opcode);

        vd_val = READ_VEC_REG<VLEN>(state, pegasus::V4);
        for (size_t i = 0; i < 8; ++i)
        {
            EXPECT_EQUAL(vd_val[i], vd_val_masked_result[i]);
        }

        state->getVectorConfig()->setVLEN(1024);
        state->getVectorConfig()->vsetVTYPE<XLEN>(state, 0x000000c0);
        state->getVectorConfig()->setVL(16);
        state->getVectorConfig()->setLMUL(1);
        // 1024 version
        std::array<uint8_t, 128> vs2_val_big = {
            0x30, 0xb3, 0xfa, 0x09, 0xb9, 0xd7, 0x8f, 0xfd, 0xb4, 0x56, 0x05, 0x59, 0x90,
            0x11, 0x3a, 0xb5, 0x20, 0x72, 0x0f, 0x79, 0x31, 0x67, 0x26, 0x9d, 0xa4, 0x14,
            0x1a, 0xc9, 0x8e, 0x54, 0x10, 0x25, 0x9f, 0x49, 0x27, 0x49, 0xac, 0xdd, 0x3a,
            0xdd, 0x89, 0xc8, 0xb0, 0x39, 0x0d, 0xb4, 0xa7, 0xf5, 0xbf, 0x64, 0xba, 0x59,
            0x24, 0x6c, 0x91, 0x7d, 0x12, 0xa9, 0x48, 0x09, 0x81, 0x59, 0xbb, 0x05, 0x18,
            0x71, 0x54, 0x89, 0xb5, 0xa8, 0x2a, 0x7d, 0x97, 0x0e, 0x9f, 0xd9, 0x31, 0x17,
            0x90, 0x75, 0xb2, 0x6a, 0x65, 0x39, 0x17, 0x72, 0x3c, 0x5d, 0x90, 0xd6, 0x35,
            0x49, 0x36, 0x11, 0x2a, 0xa5, 0x9b, 0x19, 0x81, 0xc9, 0xb5, 0x55, 0x90, 0x9d,
            0xaa, 0xcf, 0x45, 0xf9, 0x9f, 0xac, 0xbd, 0xb5, 0x04, 0xb5, 0x14, 0xd9, 0x11,
            0x2e, 0xab, 0xfd, 0x13, 0x7e, 0x62, 0x89, 0x09, 0xd2, 0x10, 0xc5};
        WRITE_VEC_REG<std::array<uint8_t, 128>>(state, pegasus::V2, vs2_val_big);
        opcode = viotamOp(pegasus::V4, pegasus::V2, 1); // unmasked
        injectInstruction(pc, opcode);

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 7);
    }

    void testVidv()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        uint32_t opcode;
        size_t vstart = 2;
        size_t vl = 8;

        state->getVectorConfig()->setVLEN(64);
        state->getVectorConfig()->setVSTART(vstart);
        state->getVectorConfig()->setVL(vl);

        VLEN v0_val = {0x8F, 0, 0, 0, 0, 0, 0, 0};
        VLEN v1_val = {8, 8, 8, 8, 8, 8, 8, 8};
        const VLEN vd_val = {8, 8, 2, 3, 8, 8, 8, 7};
        WRITE_VEC_REG<VLEN>(state, pegasus::V0, v0_val);
        WRITE_VEC_REG<VLEN>(state, pegasus::V1, v1_val);
        opcode = vidvOp(pegasus::V1, 0); // masked
        injectInstruction(pc, opcode);

        v1_val = READ_VEC_REG<VLEN>(state, pegasus::V1);
        for (size_t i = 0; i < 8; ++i)
        {
            EXPECT_EQUAL(v1_val[i], vd_val[i]);
        }

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 8);
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

    uint32_t vmsbfmOp(uint8_t vd, uint8_t vs2, uint8_t vm)
    {
        uint32_t opcode = 0x5000a057;
        uint8_t offset = 0;
        offset = 7;
        opcode |= vd << offset; // vd
        offset = 20;
        opcode |= vs2 << offset; // vs2
        offset = 25;
        opcode |= vm << offset; // vm
        return opcode;
    }

    uint32_t viotamOp(uint8_t vd, uint8_t vs2, uint8_t vm)
    {
        uint32_t opcode = 0x50082057;
        uint8_t offset = 0;
        offset = 7;
        opcode |= vd << offset; // vd
        offset = 20;
        opcode |= vs2 << offset; // vs2
        offset = 25;
        opcode |= vm << offset; // vm
        return opcode;
    }

    uint32_t vidvOp(uint8_t vd, uint8_t vm)
    {
        uint32_t opcode = 0x5008a057;
        uint8_t offset = 0;
        offset = 7;
        opcode |= vd << offset; // vd
        offset = 25;
        opcode |= vm << offset; // vm
        return opcode;
    }

  private:
    pegasus::PegasusInst::PtrType instPtr_ = nullptr;
};

int main()
{
    VmInstructionTester Vm_tester;

    Vm_tester.testVmandmm();
    Vm_tester.testVpopcm();
    Vm_tester.testVfirst();
    Vm_tester.testVmsbfm();
    Vm_tester.testViotam();
    Vm_tester.testVidv();

    REPORT_ERROR;
    return ERROR_CODE;
}
