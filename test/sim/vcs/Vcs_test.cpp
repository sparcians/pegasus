#include "test/sim/InstructionTester.hpp"
#include "sparta/utils/SpartaTester.hpp"

class VcsInstructionTester : public AtlasInstructionTester
{

  public:
    VcsInstructionTester() = default;

    void testVsetvl()
    {
        const uint64_t pc = 0x1000;
        const uint32_t opcode = 0x80007057;
        injectInstruction(pc, opcode);

        const atlas::AtlasState* state = getAtlasState();
        const atlas::AtlasState::SimState* sim_state = state->getSimState();

        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    void testVsetvli()
    {
        const uint64_t pc = 0x1000;
        const uint32_t opcode = 0x7057;
        injectInstruction(pc, opcode);

        const atlas::AtlasState* state = getAtlasState();
        const atlas::AtlasState::SimState* sim_state = state->getSimState();

        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 2);
    }

    void testVsetivli()
    {
        const uint64_t pc = 0x1000;
        const uint32_t opcode = 0xc0007057;
        injectInstruction(pc, opcode);

        const atlas::AtlasState* state = getAtlasState();
        const atlas::AtlasState::SimState* sim_state = state->getSimState();

        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 3);
    }

  private:
};

int main()
{
    VcsInstructionTester Vcs_tester;

    Vcs_tester.testVsetvl();
    Vcs_tester.testVsetvli();
    Vcs_tester.testVsetivli();

    REPORT_ERROR;
    return ERROR_CODE;
}
