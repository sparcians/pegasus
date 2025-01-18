import os, sys, wx, sqlite3, re, copy, json, socket, argparse, subprocess, time
import wx.grid
import wx.py.shell
from enum import IntEnum

class AtlasIDE(wx.Frame):
    def __init__(self, riscv_tests_dir, sim_exe_path):
        wx.Frame.__init__(self, None, -1, "Atlas IDE", size=wx.DisplaySize())
        self.riscv_tests_dir = riscv_tests_dir
        self.sim_exe_path = sim_exe_path

        self.vsplitter = wx.SplitterWindow(self, style=wx.SP_LIVE_UPDATE)
        self.test_tree = TestTreeCtrl(self.vsplitter, self)
        self.notebook = wx.Notebook(self.vsplitter)
        self.workspaces = [Workspace(self.notebook, self)]
        self.notebook.AddPage(self.workspaces[0], "Baseline")

        self.vsplitter.SplitVertically(self.test_tree, self.notebook)
        self.vsplitter.SetMinimumPaneSize(50)
        self.vsplitter.SetSashPosition(250)

        self.menu_bar = wx.MenuBar()
        self.workspace_menu = wx.Menu()
        self.workspace_menu.Append(1, "&New\tCtrl+N", "Create a new workspace")
        self.Bind(wx.EVT_MENU, self.__CreateWorkspace, id=1)
        self.menu_bar.Append(self.workspace_menu, "&Workspace")
        self.SetMenuBar(self.menu_bar)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.vsplitter, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def __CreateWorkspace(self, event):
        dlg = wx.TextEntryDialog(self, "Enter the workspace name", "Workspace Name")
        if dlg.ShowModal() == wx.ID_OK:
            self.workspaces.append(Workspace(self.notebook, self))
            self.notebook.AddPage(self.workspaces[-1], dlg.GetValue())
            self.notebook.SetSelection(self.notebook.GetPageCount()-1)
            self.Layout()

        dlg.Destroy()

class TestTreeCtrl(wx.TreeCtrl):
    def __init__(self, parent, frame):
        wx.TreeCtrl.__init__(self, parent, style=wx.TR_HAS_BUTTONS | wx.TR_HIDE_ROOT)

        self.frame = frame
        self.root = self.AddRoot("Tests")

        SUPPORTED_ARCHS = ["rv32", "rv64"]
        SUPPORTED_EXTENSIONS = ["mi", "si", "ui", "um", "ua", "uf", "ud"]
        riscv_tests_dir = self.frame.riscv_tests_dir

        tests = os.listdir(riscv_tests_dir)
        tests.sort()

        for arch in SUPPORTED_ARCHS:
            arch_root = self.__GetOrAppendItem(self.root, arch)
            for ext in SUPPORTED_EXTENSIONS:
                ext_root = self.__GetOrAppendItem(arch_root, ext)
                for test in tests:
                    if test.startswith(arch) and ext+"-" in test:
                        leaf = self.__GetOrAppendItem(ext_root, os.path.basename(test))
                        self.SetItemData(leaf, os.path.basename(test))

        orig_dir = os.getcwd()
        os.chdir(os.path.dirname(self.frame.sim_exe_path))

        test_py = os.path.join('..', '..', 'scripts', 'RunRiscVArchTest.py')
        test_py = os.path.abspath(test_py)
        assert os.path.isfile(test_py), 'Cannot find test script: ' + test_py

        self.__CaptureBaseline(test_py, 'rv32')
        #self.__CaptureBaseline(test_py, 'rv64')

        os.chdir(orig_dir)

        self.Bind(wx.EVT_RIGHT_DOWN, self.__OnRightClick)

    def __GetOrAppendItem(self, parent, text):
        item, cookie = self.GetFirstChild(parent)
        while item:
            if self.GetItemText(item) == text:
                return item
            item, cookie = self.GetNextChild(parent, cookie)
        return self.AppendItem(parent, text)

    def __CaptureBaseline(self, test_py, arch):
        arch_node = self.GetFirstChild(self.root)[0]
        while arch_node:
            if self.GetItemText(arch_node) == arch:
                break
            arch_node = self.GetNextSibling(arch_node)

        if not arch_node or not arch_node.IsOk():
            return

        test_cmd = '{} {} {}'.format(test_py, arch, self.frame.riscv_tests_dir)

        print ('Running all RISC-V tests for {} to establish a baseline...'.format(arch))
        proc = subprocess.Popen(test_cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = proc.communicate()
        if proc.returncode != 0:
            print ('Failed to capture baseline tests for', arch)
            self.Delete(arch_node)
            return

        passing = []
        capture_passing = False

        failing = []
        capture_failing = False

        for line in stdout.splitlines():
            line = line.decode('utf-8')
            if line.strip() == 'PASSED:':
                capture_passing = True
                capture_failing = False
                continue

            if line.strip() == 'FAILED:':
                capture_passing = False
                capture_failing = True
                continue

            if capture_passing and line:
                passing.append(line.strip())
                continue

            if capture_failing and line:
                failing.append(line.strip())
                continue

        # Recursively walk every node in the tree, and compare its user data to the passing/failing lists.
        # The goal is to give all failing tests red text.
        def RecurseSetItemColor(item, failing):
            if self.GetItemData(item) in failing:
                self.SetItemTextColour(item, 'red')

            child, cookie = self.GetFirstChild(item)
            while child:
                RecurseSetItemColor(child, failing)
                child, cookie = self.GetNextChild(item, cookie)

        RecurseSetItemColor(arch_node, failing)

    def __OnRightClick(self, event):
        item = self.GetSelection()
        if not item or not item.IsOk():
            return

        test = self.GetItemData(item)
        if not test:
            return

        page_idx = self.frame.notebook.GetSelection()
        workspace_name = self.frame.notebook.GetPageText(page_idx)

        menu = wx.Menu()
        menu.Append(1, "Load test in active workspace ({})".format(workspace_name))

        self.Bind(wx.EVT_MENU, self.__LoadTestInActiveWorkspace, id=1)
        self.PopupMenu(menu)

    def __LoadTestInActiveWorkspace(self, event):
        item = self.GetSelection()
        test = self.GetItemData(item)
        selected_tab = self.frame.notebook.GetSelection()
        workspace = self.frame.workspaces[selected_tab]
        workspace.LoadTest(test)

class Workspace(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.frame = frame

        self.landing_page_panel = wx.Panel(self)
        self.landing_page_info = wx.StaticText(self.landing_page_panel, label="Right-click on a test to view/debug")

        mono14bold = wx.Font(14, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.landing_page_info.SetFont(mono14bold)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.AddStretchSpacer()
        sizer.Add(self.landing_page_info, 0, wx.ALIGN_CENTER)
        sizer.AddStretchSpacer()
        self.landing_page_panel.SetSizer(sizer)

        self.workspace_panel = wx.Panel(self)
        self.inst_editor = InstEditor(self.workspace_panel, frame)
        self.inst_viewer = InstViewer(self.workspace_panel, frame)
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.inst_editor, 1, wx.EXPAND)
        sizer.Add(self.inst_viewer, 1, wx.EXPAND)
        self.workspace_panel.SetSizer(sizer)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.landing_page_panel, 1, wx.EXPAND)
        self.workspace_panel.Hide()
        self.SetSizer(sizer)
        self.Layout()

    def LoadTest(self, test):
        self.landing_page_panel.Hide()
        self.workspace_panel.Show()
        self.inst_editor.LoadTest(test)
        self.inst_viewer.LoadTest(test)
        self.GetSizer().Clear()
        self.GetSizer().Add(self.workspace_panel, 1, wx.EXPAND)
        self.Layout()

class InstEditor(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.SetBackgroundColour('blue')
        self.frame = frame

    def LoadTest(self, test):
        pass

class Interactivity(IntEnum):
    PostInit = 0
    PreExecute = 1
    PreException = 2
    PostExecute = 3
    SimFinished = 4

class InstViewer(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.SetBackgroundColour('green')
        self.frame = frame

    def LoadTest(self, test):
        test_path = os.path.join(self.frame.riscv_tests_dir, test)

        sim_endpoint = SimEndpoint()
        sim_endpoint.Bind(Interactivity.PostInit, self.__OnPostInit)
        sim_endpoint.Bind(Interactivity.PreExecute, self.__OnPreExecute)
        sim_endpoint.Bind(Interactivity.PreException, self.__OnPreException)
        sim_endpoint.Bind(Interactivity.PostExecute, self.__OnPostExecute)
        sim_endpoint.Bind(Interactivity.SimFinished, self.__OnSimFinished)
        sim_endpoint.RunSimulation(self.frame.sim_exe_path, test_path)

    def __OnPostInit(self, obj):
        print ('PostInit')

    def __OnPreExecute(self, obj):
        print ('PreExecute')

    def __OnPreException(self, obj):
        print ('PreException')

    def __OnPostExecute(self, obj):
        print ('PostExecute')

    def __OnSimFinished(self, obj):
        print ('SimFinished')

class SocketResponse:
    class Success:
        def __init__(self, s):
            self.__response = s

        @property
        def value(self):
            return self.__response

        def __str__(self):
            return self.value

    class Warning:
        def __init__(self, w):
            self.__response = w

        @property
        def value(self):
            return self.__response

    class Error:
        def __init__(self, e):
            self.__response = e

        @property
        def value(self):
            return self.__response

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
        self.sock.send(message.encode())

    def receive(self):
        return self.sock.recv(1024).decode()

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

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Atlas IDE')
    parser.add_argument('--riscv-tests-dir', required=True, help='Path to the RISC-V tests directory')
    parser.add_argument('--sim-exe-path', required=True, help='Path to the RISC-V simulator executable')
    args = parser.parse_args()

    riscv_tests_dir = args.riscv_tests_dir
    assert os.path.isdir(riscv_tests_dir), 'Invalid RISC-V tests directory: ' + riscv_tests_dir
    riscv_tests_dir = os.path.abspath(riscv_tests_dir)

    sim_exe_path = args.sim_exe_path
    assert os.path.isfile(sim_exe_path), 'Invalid simulator executable: ' + sim_exe_path
    sim_exe_path = os.path.abspath(sim_exe_path)

    app = wx.App()

    # Enable frame inspection
    #import wx.lib.inspection
    #wx.lib.inspection.InspectionTool().Show()

    frame = AtlasIDE(riscv_tests_dir, sim_exe_path)
    frame.Show()
    app.MainLoop()
