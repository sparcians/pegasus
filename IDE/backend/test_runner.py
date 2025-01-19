from backend.sim_wrapper import SimWrapper

class TestHandler:
    def __init__(self, sim):
        self.orig_csrs = sim.GetRegisterValues(3)
        self.inst_results = []

    def HandlePreExecute(self, sim):
        self.inst_results.append(TestHandler.InstHandler())
        self.inst_results[-1].HandlePreExecute(sim)

    def HandlePreException(self, sim):
        self.inst_results[-1].HandlePreException(sim)

    def HandlePostExecute(self, sim):
        self.inst_results[-1].HandlePostExecute(sim)

    def ReportResults(self, sim_state, test_results):
        #self.workload_exit_code = kwargs['workload_exit_code']
        #self.test_passed = kwargs['test_passed']
        #self.sim_stopped = kwargs['sim_stopped']
        #self.inst_count = kwargs['inst_count']
        pass

    class InstHandler:
        def __init__(self):
            self.pc = None
            self.dasm = None
            self.exception_cause = None
            self.pre_exception_csrs = None
            self.post_exception_csrs = None

        def HandlePreExecute(self, sim):
            inst = sim.GetCurrentInst()
            self.pc = sim.GetPC()
            self.dasm = inst.dasm

        def HandlePreException(self, sim):
            self.exception_cause = sim.GetUnhandledException()
            self.pre_exception_csrs = sim.GetRegisterValues(3)

        def HandlePostExecute(self, sim):
            if self.exception_cause is not None:
                self.post_exception_csrs = sim.GetRegisterValues(3)

def RunRiscvTestsAndReportResults(riscv_tests_dir, sim_exe_path, tests, test_results):
    for test in tests:
        with SimWrapper(riscv_tests_dir, sim_exe_path, test) as sim:
            # We want to run the simulation and get all the PC/disasm pairs to
            # generate a ListCtrl to show what instructions got executed before
            # the simulation finished or failed.
            #
            # Break on pre-execute
            #   - Get PC and disasm, store in a dict keyed by the inst uid
            #   - Check if this inst has python code to override
            #     - If so, reroute the rd/rs1/rs2/imm values and run the pycode in the shell
            #       and tell the Atlas sim to "go right to the finish ActionGroup"
            #     - If not, "cont"
            #
            # Break on pre-exception
            #   - TODO: Check if this exception code has been rewritten in python, reroute, etc.
            #   - Store "before" CSRs
            #
            # Break on post-execute
            #   - Get result (exception or not) and key based on inst uid
            #   - If exception, get CSRs "after" and note any differences

            sim.BreakOnPreExecute()
            sim.BreakOnPreException()
            sim.BreakOnPostExecute()

            test_handler = TestHandler(sim)
            sim_state = None
            while True:
                last_response = sim.Continue()
                assert isinstance(last_response, str)

                if last_response == 'pre_execute':
                    test_handler.HandlePreExecute(sim)
                elif last_response == 'pre_exception':
                    test_handler.HandlePreException(sim)
                elif last_response == 'post_execute':
                    test_handler.HandlePostExecute(sim)
                elif last_response in ('no_response', 'sim_finished', ''):
                    break

                sim_state = sim.GetSimState()

            import pdb; pdb.set_trace()
            test_handler.ReportResults(sim_state, test_results)
