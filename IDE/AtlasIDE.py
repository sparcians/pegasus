import os, wx, argparse
from editor.test_tree import TestTreeCtrl
from editor.test_config import TestConfig
from editor.test_results import TestResults
from backend.test_runner import RunRiscvTestsAndReportResults
from backend.workspace import Workspace

class AtlasIDE(wx.Frame):
    def __init__(self, riscv_tests_dir, sim_exe_path):
        wx.Frame.__init__(self, None, -1, "Atlas IDE", size=wx.DisplaySize())
        self.riscv_tests_dir = riscv_tests_dir
        self.sim_exe_path = sim_exe_path
        self.test_results = TestResults(self)

        self.vsplitter = wx.SplitterWindow(self, style=wx.SP_LIVE_UPDATE)
        self.test_tree = TestTreeCtrl(self.vsplitter, self)
        self.notebook = wx.Notebook(self.vsplitter)
        self.workspaces = [Workspace(self.notebook, self)]
        self.notebook.AddPage(self.workspaces[0], "Workspace 1")

        self.vsplitter.SplitVertically(self.test_tree, self.notebook)
        self.vsplitter.SetMinimumPaneSize(50)
        self.vsplitter.SetSashPosition(250)

        self.menu_bar = wx.MenuBar()

        # Workspace -> New
        self.workspace_menu = wx.Menu()
        self.workspace_menu.Append(1, "&New\tCtrl+N", "Create a new workspace")
        self.menu_bar.Append(self.workspace_menu, "&Workspace")
        self.Bind(wx.EVT_MENU, self.__CreateWorkspace, id=1)

        # Tests -> Run
        self.tests_menu = wx.Menu()
        self.tests_menu.Append(2, "&Run\tF5", "Run the test manager")
        self.menu_bar.Append(self.tests_menu, "&Tests")
        self.Bind(wx.EVT_MENU, self.__LaunchTestConfig, id=2)

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

    def __LaunchTestConfig(self, event):
        test_cfg = TestConfig(self)
        if test_cfg.ShowModal() == wx.ID_OK:
            tests = test_cfg.GetTests(self.riscv_tests_dir)
            test_cfg.Destroy()
            RunRiscvTestsAndReportResults(self.riscv_tests_dir, self.sim_exe_path, tests, self.test_results)
        else:
            test_cfg.Destroy()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Atlas IDE')
    parser.add_argument('--riscv-tests-dir', required=True, help='Path to the RISC-V tests directory')
    parser.add_argument('--sim-exe-path', required=True, help='Path to the RISC-V simulator executable')
    parser.add_argument('--inspect-frame', action='store_true', help='Enable frame inspection (developer use only)')
    args = parser.parse_args()

    riscv_tests_dir = args.riscv_tests_dir
    assert os.path.isdir(riscv_tests_dir), 'Invalid RISC-V tests directory: ' + riscv_tests_dir
    riscv_tests_dir = os.path.abspath(riscv_tests_dir)

    sim_exe_path = args.sim_exe_path
    assert os.path.isfile(sim_exe_path), 'Invalid simulator executable: ' + sim_exe_path
    sim_exe_path = os.path.abspath(sim_exe_path)

    app = wx.App()

    if args.inspect_frame:
        import wx.lib.inspection
        wx.lib.inspection.InspectionTool().Show()

    frame = AtlasIDE(riscv_tests_dir, sim_exe_path)
    frame.Show()
    app.MainLoop()

# IDE/
#   AtlasIDE.py
#   backend/
#     sim_endpoint.py
#       class SocketResponse
#       class SimSocket
#     sim_stepper.py
#       class SimStepper

