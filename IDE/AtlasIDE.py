import os, wx, argparse
from editor.test_tree import TestTreeCtrl
from editor.test_config import TestConfig
from editor.test_results import TestResults
from editor.pysim_debugger import PySimDebugger
from backend.workspace import Workspace

class AtlasIDE(wx.Frame):
    def __init__(self, riscv_tests_dir, sim_exe_path):
        wx.Frame.__init__(self, None, -1, "Atlas IDE")
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

        # File -> New Workspace
        self.workspace_menu = wx.Menu()
        self.workspace_menu.Append(1, "&New Workspace\tCtrl+N", "Create a new workspace")
        self.menu_bar.Append(self.workspace_menu, "&File")
        self.Bind(wx.EVT_MENU, self.__CreateWorkspace, id=1)

        # Debug -> Debug Test
        self.debug_menu = wx.Menu()
        self.debug_menu.Append(2, "&Debug Test\tF5", "Run the test manager")
        self.menu_bar.Append(self.debug_menu, "&Debug")
        self.Bind(wx.EVT_MENU, self.__LaunchDebugger, id=2)

        self.SetMenuBar(self.menu_bar)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.vsplitter, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()
        self.Maximize()

    def __CreateWorkspace(self, event):
        dlg = wx.TextEntryDialog(self, "Enter the workspace name", "Workspace Name")
        if dlg.ShowModal() == wx.ID_OK:
            self.workspaces.append(Workspace(self.notebook, self))
            self.notebook.AddPage(self.workspaces[-1], dlg.GetValue())
            self.notebook.SetSelection(self.notebook.GetPageCount()-1)
            self.Layout()

        dlg.Destroy()

    def __LaunchDebugger(self, event):
        page_idx = self.notebook.GetSelection()
        active_workspace = self.notebook.GetPage(page_idx)
        dlg = PySimDebugger(active_workspace)
        dlg.ShowModal()
        dlg.Destroy()

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

