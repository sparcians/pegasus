#include "test/sim/InstructionTester.hpp"
#include "core/VecElements.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "mavis/Mavis.h"
#include <typeinfo>

class VredInstructionTester : public PegasusInstructionTester
{
  public:
    using VLEN = std::array<uint8_t, 8>;
    using XLEN = uint64_t;
    using WLEN = int16_t; // 16-bit widened accumulator/result

    VredInstructionTester() = default;

    void testVredsumvs1()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        const uint32_t vd = 10; // scalar destination (reduction result)
        const uint32_t vs1 = 1; // reduction initial value (accumulator)
        const uint32_t vs2 = 2; // vector source
        uint32_t opcode;

        // Configure vector unit
        state->getVectorConfig()->setVLEN(64);
        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(8);   // VL = 8 elements
        state->getVectorConfig()->setLMUL(1); // LMUL = 1
        state->getVectorConfig()->setSEW(8);  // SEW = 8 bits

        // Initial values
        VLEN vs1_val = {255, 0, 0, 0, 0, 0, 0, 0}; // Start with scalar accumulator = 10
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};
        XLEN expected_sum = vs1_val[0];
        for (auto val : vs2_val)
        {
            if (expected_sum >= 256)
                expected_sum = expected_sum - 256; // to handle to wrap around
            expected_sum += val;
        }

        // Write input values
        WRITE_VEC_REG<VLEN>(state, vs1, vs1_val); // vector register
        WRITE_VEC_REG<VLEN>(state, vs2, vs2_val); // vector register

        // Encode and inject vredsum.vs
        opcode = vredsumvsOp(vd, vs1, vs2, 1, false); // vm = 1 (unmasked)
        injectInstruction(pc, opcode);

        // Read result
        auto vd_val = READ_VEC_REG<VLEN>(state, vd);
        EXPECT_EQUAL(vd_val[0], expected_sum); // expected_sum --> 35 (0010 0011)

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    void testVwredsumvs1()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        const uint32_t vd = 10; // destination scalar (wide result)
        const uint32_t vs1 = 1; // wide scalar accumulator
        const uint32_t vs2 = 2; // vector input (narrow)
        uint32_t opcode;

        // Configure vector unit
        state->getVectorConfig()->setVLEN(64);
        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(8);   // VL = 8 elements
        state->getVectorConfig()->setLMUL(1); // LMUL = 1
        state->getVectorConfig()->setSEW(8);  // SEW = 8-bit

        // Set input values
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};   // vector of int8_t
        VLEN vs1_val = {255, 0, 0, 0, 0, 0, 0, 0}; // initial accumulator
        WLEN expected_sum = vs1_val[0];
        for (auto val : vs2_val)
            expected_sum += static_cast<WLEN>(val);

        // Write inputs
        WRITE_VEC_REG<VLEN>(state, vs1, vs1_val); // scalar accumulator
        WRITE_VEC_REG<VLEN>(state, vs2, vs2_val); // vector input

        // Encode and inject instruction
        opcode = vredsumvsOp(vd, vs1, vs2, 1, true); // vm = 1 (unmasked)
        injectInstruction(pc, opcode);

        // Validate result
        auto vd_val = READ_VEC_REG<VLEN>(state, vd);
        EXPECT_EQUAL(vd_val[0], expected_sum); // final expected sum should be 291 (0001 0010 0011)

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 2);
    }

    void testVfredosumvs()
    {
        // using FLEN = float;
        using VF32 = std::array<float, 8>;

        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        const uint32_t vd = 10;
        const uint32_t vs1 = 1;
        const uint32_t vs2 = 2;
        uint32_t opcode;

        // Configure vector unit
        state->getVectorConfig()->setVLEN(1024); // large enough for 8 floats
        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(4); // process 4 elements
        state->getVectorConfig()->setLMUL(1);
        state->getVectorConfig()->setSEW(32); // 32-bit float

        // VF32 vs1_val = {1.5f};                   // Initial accumulator = 1.0
        // VF32 vs2_val = {2.0f, 3.0f, 4.0f, 5.0f}; // Values to sum

        VF32 vs2_val = {1.5f, 2.25f, 3.125f, 4.5f}; // float vector
        VF32 vs1_val = {5.0};                       // initial double accumulator

        float expected_sum = vs1_val[0];
        for (int i = 0; i < 4; ++i)
        {
            expected_sum += vs2_val[i];
        }

        // Write input values
        WRITE_VEC_REG<VF32>(state, vs1, vs1_val); // scalar accumulator
        WRITE_VEC_REG<VF32>(state, vs2, vs2_val); // vector elements

        // Encode instruction (vfredosum.vs)
        opcode = vfredosumvsOp(vd, vs1, vs2, 1); // vm = 1 (unmasked)
        injectInstruction(pc, opcode);

        // Read and check result
        auto vd_val = READ_VEC_REG<VF32>(state, vd);
        EXPECT_EQUAL(vd_val[0], expected_sum);

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 3); // third instruction
    }

    void testVfwredsumvs()
    {
        using VF32 = std::array<float, 8>;  // SEW = 32-bit float input
        using VF64 = std::array<double, 8>; // Widened accumulator/result

        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        const uint32_t vd = 10; // scalar destination (double)
        const uint32_t vs1 = 1; // wide scalar accumulator (double)
        const uint32_t vs2 = 2; // narrow vector input (float)
        uint32_t opcode;

        // Configure vector unit for floating-point 32-bit input, widened to 64-bit
        state->getVectorConfig()->setVLEN(1024);
        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(4); // Only 4 elements to keep it simple
        state->getVectorConfig()->setLMUL(1);
        state->getVectorConfig()->setSEW(32); // Input element is 32-bit float

        // Inputs
        VF32 vs2_val = {1.5f, 2.25f, 3.125f, 4.5f}; // float vector
        // VF64 vs1_val = {5.0};                      // initial double accumulator
        VF32 vs1_val = {5.0f};
        double expected_sum = static_cast<double>(vs1_val[0]);
        for (int i = 0; i < 4; ++i)
        {
            expected_sum += static_cast<double>(vs2_val[i]); // Widen and accumulate
        }

        // Write values to registers
        WRITE_VEC_REG<VF32>(state, vs1, vs1_val); // wide accumulator
        WRITE_VEC_REG<VF32>(state, vs2, vs2_val); // narrow float vector input

        // Encode instruction: vfwredsum.vs
        opcode = vfwredosumvsOp(vd, vs1, vs2, 1); // vm = 1 (unmasked)
        injectInstruction(pc, opcode);

        // Read result from vector register
        auto vd_val = READ_VEC_REG<VF64>(state, vd);
        EXPECT_EQUAL(vd_val[0], expected_sum); // Should be 5.0 + 11.375 = 16.375

        uint64_t actual_bits;
        std::memcpy(&actual_bits, &vd_val[0], sizeof(actual_bits));

        EXPECT_EQUAL(actual_bits, 0x4030600000000000ULL);

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 4); // fourth instruction
    }

    // vredsum.vs encoding helper
    uint32_t vredsumvsOp(uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t vm, bool isWideningEnabled)
    {
        uint32_t opcode = 0;
        uint8_t offset = 0;
        opcode |= 0x57 << offset; // opcode = 0x57 (vector)
        offset += 7;
        opcode |= rd << offset; // rd (scalar result)
        offset += 5;
        if (isWideningEnabled)
        {
            opcode |= 0 << offset; // funct3 = 000
        }
        else
        {
            opcode |= 2 << offset; // funct3 = 010
        }
        offset += 3;
        opcode |= rs1 << offset; // rs1 (accumulator)
        offset += 5;
        opcode |= rs2 << offset; // rs2 (vector source)
        offset += 5;
        opcode |= vm << offset; // vm
        offset += 1;
        if (isWideningEnabled)
        {
            opcode |= 0x31 << offset; // funct6 = 110001 for vwredsum.vs
        }
        else
        {
            opcode |= 0x00 << offset; // funct6 = 00 for vredsum.vs
        }
        offset += 6;
        EXPECT_EQUAL(offset, 32);
        return opcode;
    }

    uint32_t vfredosumvsOp(uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t vm)
    {
        uint32_t opcode = 0;
        uint8_t offset = 0;
        opcode |= 0x57 << offset; // base opcode
        offset += 7;
        opcode |= rd << offset;
        offset += 5;
        opcode |= 1 << offset; // funct3 = 001 for float reductions
        offset += 3;
        opcode |= rs1 << offset;
        offset += 5;
        opcode |= rs2 << offset;
        offset += 5;
        opcode |= vm << offset;
        offset += 1;
        opcode |= 0x03 << offset; // funct6 = 000011 (vfredosum)
        offset += 6;
        EXPECT_EQUAL(offset, 32);
        return opcode;
    }

    uint32_t vfwredosumvsOp(uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t vm)
    {
        uint32_t opcode = 0;
        uint8_t offset = 0;
        opcode |= 0x57 << offset; // base opcode
        offset += 7;
        opcode |= rd << offset;
        offset += 5;
        opcode |= 1 << offset; // funct3 = 001 for float reductions
        offset += 3;
        opcode |= rs1 << offset;
        offset += 5;
        opcode |= rs2 << offset;
        offset += 5;
        opcode |= vm << offset;
        offset += 1;
        opcode |= 0x33 << offset; // funct6 = 110011 (vfredosum)
        offset += 6;
        EXPECT_EQUAL(offset, 32);
        return opcode;
    }

  private:
    pegasus::PegasusInst::PtrType instPtr_ = nullptr;
};

int main()
{
    VredInstructionTester tester;
    tester.testVredsumvs1();
    tester.testVwredsumvs1();
    tester.testVfredosumvs();
    tester.testVfwredsumvs();

    REPORT_ERROR;
    return ERROR_CODE;
}
