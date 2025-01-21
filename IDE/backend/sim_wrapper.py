import os, subprocess, sys
from backend.dtypes import JsonConverter
from backend.sim_api import BrokenPipeResponse

# This class is to be used as follows:
#
#   with SimWrapper(riscv_tests_dir, sim_exe_path, test_name) as sim:
#       ...
#
# The sim object will be used to interact with the simulator, and the
# C++ simulation is running for as long as the 'with' block is active.
class SimWrapper:
    def __init__(self, riscv_tests_dir, sim_exe_path, test_name):
        self.riscv_tests_dir = riscv_tests_dir
        self.sim_exe_path = sim_exe_path
        self.test_name = test_name
        self.return_dir = os.getcwd()
        self.endpoint = SimEndpoint()

    def UnscopedEnter(self):
        os.chdir(os.path.dirname(self.sim_exe_path))
        program_path = "./atlas"
        program_args = ["--interactive", f"{self.riscv_tests_dir}/{self.test_name}"]
        return self if self.endpoint.start_server(program_path, *program_args) else None

    def UnscopedExit(self):
        if self.endpoint.process:
            self.endpoint.close()
        os.chdir(self.return_dir)

    def __enter__(self):
        return self.UnscopedEnter()

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.UnscopedExit()

    # The following commands are supported:
    #
    # 'pc'                                      -> returns the current PC
    # 'status'                                  -> returns the SimState
    # 'inst'                                    -> returns the current AtlasInst
    # 'exception'                               -> returns the cause (error code) of the last exception
    # 'regdump <groupp num>'                    -> returns the register dump for the specified group
    # 'regread <group num> <reg id>'            -> returns the specified register value (hex)
    # 'regwrite <group num> <reg id> <val>'     -> writes the specified value to the specified register
    # 'regdmiwrite <group num> <addr> <val>'    -> writes the specified value to the specified DMI address
    # 'cont'                                    -> continue execution to next breakpoint or end of simulation
    # 'break info'                              -> returns the list of breakpoints
    # 'break delete'                            -> deletes all breakpoints
    # 'break exception'                         -> breaks on the next exception
    # 'break disable'                           -> disables all breakpoints
    # 'break enable'                            -> enables all breakpoints
    # 'break <inst>'                            -> breaks on the specified instruction
    # 'break delete <n>'                        -> deletes the breakpoint with the specified number
    # 'break disable <n>'                       -> disables the breakpoint with the specified number
    # 'break enable <n>'                        -> enables the breakpoint with the specified number
    # 'break pc <pc>'                           -> breaks on the specified PC
    # 'break pre_execute'                       -> breaks at the start of preExecute() in C++
    # 'break pre_exception'                     -> breaks at the start of preException() in C++
    # 'break post_execute'                      -> breaks at the start of postExecute() in C++
    # 'break pc <comparator> <pc>'              -> breaks on the specified PC with the specified comparator
    #                                              -> break pc >= 1000
    #                                              -> break pc == 5000
    #                                              -> (==, !=, <, <=, >, >=)
    def PingAtlas(self, command):
        return self.endpoint.request(command)

# This class runs an Atlas simulation in the background and provides basic
# low-level communication with the simulator.
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

        if self.process:
            print ('Started simulator with PID ' + str(self.process.pid))

        return self.process is not None

    def request(self, request):
        try:
            self.__send(request)
        except BrokenPipeError:
            return BrokenPipeResponse()

        response = None
        while response is None:
            recvd = self.__receive()
            if recvd == '':
                return ''

            # The ATLAS_IDE_RESPONSE keyword is used to distinguish between
            # the actual response and other messages that the simulator may
            # send to stdout. Since we are in a while loop, we can just take
            # the last response we see with this keyword (we just called the
            # 'send' method which always gets a response sent to stdout by C++).
            if recvd.find('ATLAS_IDE_RESPONSE: ') != -1:
                response = recvd.split('ATLAS_IDE_RESPONSE: ')[1].strip()

        return JsonConverter.ConvertResponse(response)

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
