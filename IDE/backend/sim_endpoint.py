from enum import IntEnum
import os, json, socket, subprocess, time

class Interactivity(IntEnum):
    PostInit = 0
    PreExecute = 1
    PreException = 2
    PostExecute = 3
    SimFinished = 4

# This class bridges the IDE python code and a running Atlas simulation instance.
# It it used for a variety of purposes, including querying register values, overwriting
# register values, stepping through instructions one at a time, and more.
class SimEndpoint:
    def __init__(self):
        self.interactivity_callbacks = {}

    def Bind(self, interactivity, callback):
        assert interactivity in Interactivity, 'Invalid interactivity type'
        self.interactivity_callbacks[interactivity] = callback

    def Unbind(self, interactivity):
        if interactivity in self.interactivity_callbacks:
            del self.interactivity_callbacks[interactivity]

    def RunSimulation(self, sim_exe_path, test_path):
        # Start the Atlas simulation in interactive mode.
        sim_cmd = '{} --interactive {}'.format(sim_exe_path, test_path)

        orig_dir = os.getcwd()
        os.chdir(os.path.dirname(sim_exe_path))
        proc = subprocess.Popen(sim_cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        os.chdir(orig_dir)

        class SimStepper:
            def __init__(self):
                import pdb; pdb.set_trace()
                self.sim_socket = SimSocket('localhost', 65432)

            def SendRequest(self, request, formatter=str):
                self.sim_socket.send(request)
                response = self.sim_socket.receive().strip()

                if 'ERROR:' in response:
                    return SocketResponse.Error(response.replace('ERROR:', '').strip())

                if 'WARNING:' in response:
                    return SocketResponse.Warning(response.replace('WARNING:', '').strip())

                return SocketResponse.Success(formatter(response.strip()))

            def GetSimStatus(self):
                response = self.SendRequest('status')
                if isinstance(response, SocketResponse.Success):
                    return json.loads(response.value)

                return response

            def SimFinished(self):
                return self.GetSimStatus()['sim_stopped']

            def StepSim(self):
                if not self.SimFinished():
                    response = self.SendRequest('cont')
                    if isinstance(response, SocketResponse.Success):
                        return response.value

            def GetFullRegDump(self):
                with open(self.SendRequest('dump 0').value, 'r') as fin:
                    regdump0 = json.load(fin)

                with open(self.SendRequest('dump 1').value, 'r') as fin:
                    regdump1 = json.load(fin)

                with open(self.SendRequest('dump 2').value, 'r') as fin:
                    regdump2 = json.load(fin)

                with open(self.SendRequest('dump 3').value, 'r') as fin:
                    regdump3 = json.load(fin)

                regs = {
                    0: regdump0,
                    1: regdump1,
                    2: regdump2,
                    3: regdump3
                }

                return regs

            def GetCurrentInst(self):
                response = self.SendRequest('inst')
                if isinstance(response, SocketResponse.Success):
                    import pdb; pdb.set_trace()
                    response = response.value.replace('\t', ' ').replace('\n', '')
                    return json.loads(response.value)

                return response

            def Close(self):
                self.sim_socket.send('exit')
                self.sim_socket.close()
                self.sim_socket = None

            def __del__(self):
                import pdb; pdb.set_trace()
                if self.sim_socket:
                    self.sim_socket.close()

        stepper = SimStepper()

        if Interactivity.PostInit in self.interactivity_callbacks:
            # We are implicitly at the postInit() method in C++ right now,
            # so we don't have to set any breakpoints.
            cb_data = {'regs': stepper.GetFullRegDump()}
            self.interactivity_callbacks[Interactivity.PostInit](cb_data)

        if Interactivity.PreExecute in self.interactivity_callbacks:
            response = stepper.SendRequest('break action pre_execute')
            assert not isinstance(response, SocketResponse.Error), response.value

        if Interactivity.PreException in self.interactivity_callbacks:
            response = stepper.SendRequest('break action pre_exception')
            assert not isinstance(response, SocketResponse.Error), response.value

        if Interactivity.PostExecute in self.interactivity_callbacks:
            response = stepper.SendRequest('break action post_execute')
            assert not isinstance(response, SocketResponse.Error), response.value

        if Interactivity.SimFinished in self.interactivity_callbacks:
            # The --interactive simulations always break at the end of sim
            pass

        while True:
            # Run to first breakpoint or to simulation end.
            subphase = stepper.StepSim()

            if subphase == 'pre_execute':
                if Interactivity.PreExecute in self.interactivity_callbacks:
                    cb_data = {'inst': stepper.GetCurrentInst(), 'regs': stepper.GetFullRegDump()}
                    self.interactivity_callbacks[Interactivity.PreExecute](cb_data)

            elif subphase == 'pre_exception':
                if Interactivity.PreException in self.interactivity_callbacks:
                    cb_data = {'inst': stepper.GetCurrentInst(), 'regs': stepper.GetFullRegDump()}
                    self.interactivity_callbacks[Interactivity.PreException](cb_data)

            elif subphase == 'post_execute':
                if Interactivity.PostExecute in self.interactivity_callbacks:
                    cb_data = {'regs': stepper.GetFullRegDump()}
                    self.interactivity_callbacks[Interactivity.PostExecute](cb_data)

            else:
                # Simulation has ended
                break

        if Interactivity.SimFinished in self.interactivity_callbacks:
            cb_data = {'regs': stepper.GetFullRegDump(), 'status': stepper.GetSimStatus()}
            self.interactivity_callbacks[Interactivity.SimFinished](cb_data)

        stepper.Close()

# This class handles socket communication between the IDE and the running
# Atlas simulation instance.
class SimSocket:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self._sock = None

    def send(self, message):
        message = message.encode().strip()
        print ('-> Sending request: {}'.format(message))
        self.sock.send(message)

    def receive(self):
        response = self.sock.recv(1024).decode().strip()
        print ('<- Received response: {}'.format(response))
        return response

    def close(self):
        if self._sock:
            self._sock.close()
            self._sock = None

    def __del__(self):
        self.close()

    @property
    def sock(self):
        num_tries = 10
        while num_tries > 0 and self._sock is None:
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.connect((self.host, self.port))
                self._sock = sock
            except:
                print ('Failed to connect to simulator. Retrying...')
                num_tries -= 1
                time.sleep(1)

        class NopSock:
            def send(self, message):
                print ('Cannot send. Socket is not open.')
                pass

            def recv(self):
                print ('Cannot receive. Socket is not open')
                return b''

            def close(self):
                print ('Cannot close. Socket is not open.')
                pass

        return self._sock if self._sock else NopSock()
