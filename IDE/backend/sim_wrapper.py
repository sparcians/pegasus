import os, subprocess, socket, json

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
        self.socket_server = SocketServer('localhost', 65432)

    def __enter__(self):
        os.chdir(os.path.dirname(self.sim_exe_path))
        program_path = "./atlas"
        program_args = ["--interactive", f"{self.riscv_tests_dir}/{self.test_name}"]
        self.socket_server.start_server(program_path, *program_args)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.socket_server.close()
        os.chdir(self.return_dir)

    def BreakOnPreExecute(self):
        self.socket_server.request('break action pre_execute')

    def BreakOnPostExecute(self):
        self.socket_server.request('break action post_execute')

    def BreakOnPreException(self):
        self.socket_server.request('break action pre_exception')

    def GetRegisterValues(self, group_num=-1):
        if group_num == -1:
            return {
                0: self.socket_server.request('dump 0'),
                1: self.socket_server.request('dump 1'),
                2: self.socket_server.request('dump 2'),
                3: self.socket_server.request('dump 3'),
            }

        return {
            group_num: self.socket_server.request('dump {}'.format(group_num)),
        }

    def GetCurrentInst(self):
        descriptor = self.socket_server.request('inst')
        if descriptor == '{}':
            return None

        descriptor = json.loads(descriptor)
        if len(descriptor) == 0:
            return None

        return AtlasInst(**descriptor)

    def GetSimState(self):
        descriptor = self.socket_server.request('status')
        if descriptor == '{}':
            return None

        descriptor = json.loads(descriptor)
        if len(descriptor) == 0:
            return None

        return SimState(**descriptor)

    def Continue(self):
        ack = self.socket_server.request('cont')
        return json.loads(ack)['action']

class AtlasInst:
    def __init__(self, **kwargs):
        self.mnemonic = kwargs['mnemonic']
        self.dasm = kwargs['dasm_string']
        self.opcode = kwargs['opcode']
        self.opcode_size = kwargs['opcode_size']
        self.is_memory_inst = kwargs['is_memory_inst']

        if kwargs['has_immediate']:
            self.immediate = kwargs['immediate']
        else:
            self.immediate = None

        if kwargs['has_rs1']:
            self.rs1 = kwargs['rs1']
        else:
            self.rs1 = None

        if kwargs['has_rs2']:
            self.rs2 = kwargs['rs2']
        else:
            self.rs2 = None

class SimState:
    def __init__(self, **kwargs):
        self.workload_exit_code = kwargs['workload_exit_code']
        self.test_passed = kwargs['test_passed']
        self.sim_stopped = kwargs['sim_stopped']
        self.inst_count = kwargs['inst_count']

class SocketServer:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.server_socket = None
        self.client_socket = None
        self.process = None

    def start_server(self, program_path, *program_args):
        """Start the server and subprocess."""
        # Setup the socket
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server_socket.bind((self.host, self.port))
        self.server_socket.listen(1)
        print(f"Server listening on {self.host}:{self.port}")

        # Start the long-running subprocess
        self.process = subprocess.Popen(
            [program_path, *program_args],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE
        )

        # Accept incoming client connection
        self.client_socket, _ = self.server_socket.accept()
        print("Client connected.")

    def request(self, request):
        self.send(request)
        return self.receive().strip()

    def send(self, message):
        message = message.encode().strip()
        print ('-> Sending request: {}'.format(message))
        self.client_socket.send(message)

    def receive(self):
        response = self.client_socket.recv(4096).decode().strip()
        print ('<- Received response: {}'.format(response))
        return response

    def close(self):
        if self.client_socket:
            self.client_socket.close()
            self.client_socket = None
        if self.server_socket:
            self.server_socket.close()
            self.server_socket = None
        if self.process:
            self.process.terminate()
            self.process.wait()
            self.process = None
        print("Server and subprocess cleaned up.")
