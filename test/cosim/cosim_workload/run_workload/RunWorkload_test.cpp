#include "cosim/PegasusCoSim.hpp"
#include "cosim/CoSimPipeline.hpp"
#include "simdb/apps/AppManager.hpp"
#include "sparta/utils/SpartaTester.hpp"

void runCoSimWorkload(const std::string & workload)
{
    const uint64_t ilimit = 0;
    sparta::Scheduler scheduler;
    pegasus::cosim::PegasusCoSim cosim(&scheduler, ilimit, workload);

    const pegasus::HartId hart_id = 0;
    std::string exception_str;
    try
    {
        cosim.enableLogger("run.log");
        cosim.run(sparta::Scheduler::INDEFINITE);
    }
    catch (const std::exception & ex)
    {
        exception_str = ex.what();
    }

    cosim.finish();

    auto state = cosim.getPegasusState(hart_id);
    auto sim_state = state->getSimState();
    auto workload_exit_code = sim_state->workload_exit_code;

    EXPECT_EQUAL(workload_exit_code, 0);
    EXPECT_EQUAL(exception_str, "");
}

int main(int argc, char** argv)
{
    const std::string workload = argc >= 2 ? argv[1] : "";
    if (workload.empty())
    {
        std::cout << "Must supply workload!" << std::endl;
        return 1;
    }

    runCoSimWorkload(workload);

    REPORT_ERROR;
    return ERROR_CODE;
}
