#include "test/sim/InstructionTester.hpp"
#include "sparta/utils/SpartaTester.hpp"

class ViaInstructionTester : public AtlasInstructionTester
{

  public:
    ViaInstructionTester() = default;

    void testVaddvv()
    {
        vsetivli(0, 8, 0, 0, 0, 0); // vlmul = 1, sew = 8

        const uint64_t pc = 0x1000;
        const uint32_t opcode = 0x3100D7;
        injectInstruction(pc, opcode);

        const atlas::AtlasState* state = getAtlasState();
        const atlas::AtlasState::SimState* sim_state = state->getSimState();

        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 2);
    }

  private:
    void vsetivli(uint8_t rd, uint8_t avl, uint8_t vlmul, uint8_t sew, uint8_t vta, uint8_t vma)
    {
        const uint64_t pc = 0x1000;
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

        injectInstruction(pc, opcode);
    }
};

int main()
{
    ViaInstructionTester Via_tester;

    Via_tester.testVaddvv();

    REPORT_ERROR;
    return ERROR_CODE;
}
