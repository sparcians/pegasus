import os, subprocess, json

# Use case 1: We want to run the simulation one instruction at a time,
# and get the disassembly of all the instructions in the test. This
# is used to create the wx.ListCtrl in the InstViewer.
#
#     with SimWrapper(riscv_tests_dir, sim_exe_path, test_name) as sim:
#         init_regs = sim.GetRegisterValues(self, group_num=-1)
#         sim.BreakOnPostExecute()
#
#         list_items = []
#         while sim.Continue() == 'post_execute':
#             inst = sim.GetCurrentInst()
#             mnemonic = inst.mnemonic
#             dasm = inst.dasm
#             list_items.append((mnemonic, dasm))
#
#         # The simulation is finished and can take questions about
#         # final register values etc.

# Use case 2: We want to step through to a particular instruction,
# then instruct the C++ simulator NOT to run that instruction.
# Instead, we will write our inst handler in Python and reroute
# the C++ simulator right to the Finish ActionGroup.
#
#     with SimWrapper(riscv_tests_dir, sim_exe_path, test_name) as sim:
#         ... TODO cnyce ...

# Use case 3: We want to run the workspace handler code against one/many
# tests, and get the final pass/fail status of the test(s).
#
#     with SimWrapper(riscv_tests_dir, sim_exe_path, test_name) as sim:
#         sim.Continue()
#         sim_state = sim.GetSimState()
#         #  - workload_exit_code
#         #  - test_passed
#         #  - sim_stopped
#         #  - inst_count

class SimWrapper:
    def __init__(self, riscv_tests_dir, sim_exe_path, test_name):
        self.riscv_tests_dir = riscv_tests_dir
        self.sim_exe_path = sim_exe_path
        self.test_name = test_name
        self.return_dir = os.getcwd()
        self.endpoint = SimEndpoint()

    def __enter__(self):
        os.chdir(os.path.dirname(self.sim_exe_path))
        program_path = "./atlas"
        program_args = ["--interactive", f"{self.riscv_tests_dir}/{self.test_name}"]
        return self if self.endpoint.start_server(program_path, *program_args) else None

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.endpoint.process:
            self.endpoint.close()
        os.chdir(self.return_dir)

    def BreakOnPreExecute(self):
        self.endpoint.request('break action pre_execute', 'ack')

    def BreakOnPostExecute(self):
        self.endpoint.request('break action post_execute', 'ack')

    def BreakOnPreException(self):
        self.endpoint.request('break action pre_exception', 'ack')

    def GetRegisterValues(self, group_num=-1):
        if group_num == -1:
            regs = {
                0: self.endpoint.request('dump 0', 'regfile'),
                1: self.endpoint.request('dump 1', 'regfile'),
                2: self.endpoint.request('dump 2', 'regfile'),
                3: self.endpoint.request('dump 3', 'regfile'),
            }
        else:
            regs = {
                group_num: self.endpoint.request('dump {}'.format(group_num), 'regfile')
            }

        for group_num, reg_file in regs.items():
            with open(reg_file.strip(), 'r') as fin:
                regs[group_num] = json.load(fin)

        return regs

    def GetCurrentInst(self):
        descriptor = self.endpoint.request('inst')
        if descriptor == '{}':
            return None

        descriptor = json.loads(descriptor)
        if len(descriptor) == 0:
            return None

        return AtlasInst(**descriptor)

    def GetSimState(self):
        descriptor = self.endpoint.request('status')
        if descriptor == '{}':
            return None

        descriptor = json.loads(descriptor)
        if len(descriptor) == 0:
            return None

        return SimState(**descriptor)

    def Continue(self):
        sim_state = self.GetSimState()
        if sim_state.sim_stopped:
            return 'sim_finished'

        return self.endpoint.request('cont', 'action')

class AtlasInst:
    def __init__(self, **kwargs):
        self.mnemonic = kwargs['mnemonic']
        self.dasm = kwargs['dasm_string']
        self.opcode = kwargs['opcode']
        self.opcode_size = kwargs['opcode_size']
        self.is_memory_inst = kwargs['is_memory_inst']
        self.immediate = kwargs.get('immediate', None)
        self.rs1 = kwargs.get('rs1', None)
        self.rs2 = kwargs.get('rs2', None)
        self.rd = kwargs.get('rd', None)

class SimState:
    def __init__(self, **kwargs):
        self.workload_exit_code = kwargs['workload_exit_code']
        self.test_passed = kwargs['test_passed']
        self.sim_stopped = kwargs['sim_stopped']
        self.inst_count = kwargs['inst_count']

class SimEndpoint:
    def __init__(self):
        self.process = None

    def start_server(self, program_path, *program_args):
        self.process = subprocess.Popen(
            [program_path, *program_args],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE,
            text=True
        )

        ide_ready = False
        while not ide_ready:
            response = self.__receive().strip()
            if response == 'ATLAS_IDE_READY':
                ide_ready = True

        if not ide_ready:
            self.close()

        return self.process is not None

    def request(self, request, response_key=None):
        self.__send(request)

        response = None
        while response is None:
            recvd = self.__receive()
            if recvd.find('ATLAS_IDE_RESPONSE: ') != -1:
                response = recvd.split('ATLAS_IDE_RESPONSE: ')[1].strip()
                if response_key is not None:
                    try:
                        response_json = json.loads(response)
                        response = response_json[response_key]
                    except:
                        response = None

        return response

    def close(self):
        if self.process:
            self.process.terminate()
            self.process.wait()
            self.process = None

    def __send(self, message):
        self.process.stdin.write(message.strip() + '\n')
        self.process.stdin.flush()

    def __receive(self):
        return self.process.stdout.readline().strip()
