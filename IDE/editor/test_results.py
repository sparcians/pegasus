class TestInst:
    def __init__(self, pc, dasm, pycode, exception_code, csr_diffs):
        self.pc = pc
        self.dasm = dasm
        self.pycode = pycode
        self.exception_code = exception_code
        self.csr_diffs = csr_diffs

class TestResult:
    def __init__(self):
        self.init_regs = None
        self.insts = []
        self.final_sim_state = None

    def SetInitialRegisterState(self, regs):
        self.init_regs = regs

    def AppendSuccessfulInst(self, pc, dasm, pycode):
        self.insts.append(TestInst(pc, dasm, pycode, None, {}))

    def AppendExceptionInst(self, pc, dasm, pycode, exception_code, csrs_before, csrs_after):
        assert (set(csrs_before.keys()) == set(csrs_after.keys()))

        csr_diffs = {}
        for csr, _ in csrs_before.items():
            if csrs_before[csr] != csrs_after[csr]:
                csr_diffs[csr] = (csrs_before[csr], csrs_after[csr])

        self.insts.append(TestInst(pc, dasm, pycode, exception_code, csr_diffs))

    def SetFinalSimState(self, sim_state):
        self.final_sim_state = sim_state

    def AppendReport(self, test_results):
        test_results.AppendReport(self.init_regs, self.insts, self.final_sim_state)

class TestResults:
    def __init__(self, frame):
        self.frame = frame

    def AppendReport(self, init_regs, insts, final_sim_state):
        int_regs_names = ['x{}'.format(i) for i in range(32)]
        fp_regs_names = ['f{}'.format(i) for i in range(32)]
        vec_regs_names = ['v{}'.format(i) for i in range(32)]

        csr_init_vals = {}
        for name, val in init_regs.items():
            if name not in int_regs_names and name not in fp_regs_names and name not in vec_regs_names:
                csr_init_vals[name] = val

        csr_names = list(csr_init_vals.keys())
        csr_names.sort()

        print ('Initial CSR values: ')
        for csr in csr_names:
            print (f'  {csr}: {csr_init_vals[csr]}')

        print ('Instructions executed:')
        for inst in insts:
            pc = inst.pc
            dasm = inst.dasm
            print (f'{pc}:\t{dasm}')
            if inst.exception_code is not None:
                print (f'  CSR changes due to exception (code:{inst.exception_code}):')
                for csr, (before, after) in inst.csr_diffs.items():
                    # use zfill on before/after for hex()
                    before = hex(before).zfill(8)
                    after = hex(after).zfill(8)
                    print (f'    {csr}: {before} -> {after}')

        print ('Final sim state:')
        print ('  Workload exit code:', final_sim_state.workload_exit_code)
        print ('  Test passed:', final_sim_state.test_passed)
        print ('  Instruction count:', final_sim_state.inst_count)
