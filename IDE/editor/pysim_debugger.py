import wx
import wx.py.shell
from backend.sim_wrapper import SimWrapper

class MiniDebugger:
    def __init__(self, pysim_debugger):
        self.__pysim_debugger = pysim_debugger

    def BreakInst(self, mnemonic):
        self.__pysim_debugger.BreakInst(mnemonic)

    def DeleteBreakpoints(self):
        self.__pysim_debugger.DeleteBreakpoints()

    def Continue(self):
        self.__pysim_debugger.Continue()

class PySimDebugger(wx.Dialog):
    def __init__(self, workspace):
        wx.Dialog.__init__(self, None, title="PySim Debugger - {}".format(workspace.frame.GetTitle()))

        self.workspace = workspace
        self.riscv_tests_dir = workspace.frame.riscv_tests_dir
        self.sim_exe_path = workspace.frame.sim_exe_path
        self.test = workspace.frame.GetTitle()
        self.code_mgr = workspace.inst_editor.inst_impl_panel.runtime_code_mgr
        self.hsplitter = wx.SplitterWindow(self, style=wx.SP_LIVE_UPDATE)
        self.pyshell = wx.py.shell.Shell(self.hsplitter, locals={'debugger': MiniDebugger(self)})

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.hsplitter, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.SetSize(workspace.frame.GetSize())
        self.pyshell.SetSize(self.GetSize())
        self.Layout()

        self.sim_wrapper = SimWrapper(self.riscv_tests_dir, self.sim_exe_path, self.test)
        self.sim_wrapper.UnscopedEnter()

    def Destroy(self):
        self.sim_wrapper.UnscopedExit()
        super().Destroy()

    def BreakInst(self, mnemonic):
        self.sim_wrapper.PingAtlas('break {}'.format(mnemonic))

    def DeleteBreakpoints(self):
        self.sim_wrapper.PingAtlas('break delete')

    def Continue(self):
        if not self.sim_wrapper.PingAtlas('status').sim_stopped:
            action = self.sim_wrapper.PingAtlas('cont')
            if action == 'pre_execute':
                inst = self.sim_wrapper.PingAtlas('inst')
                python_code = self.code_mgr.GetPythonCode(inst.mnemonic)
                atlas_cpp_code = self.workspace.inst_editor.inst_impl_panel.GetAtlasCppCode(inst.mnemonic)
                self.__UpdateCodeViewer(python_code, atlas_cpp_code)
            elif action in ('no_response', 'sim_finished', ''):
                self.EndModal(wx.ID_OK)
        else:
            self.EndModal(wx.ID_OK)

    def __UpdateCodeViewer(self, python_code, atlas_cpp_code):
        win1 = self.hsplitter.GetWindow1()
        win2 = self.hsplitter.GetWindow2()

        if win1 is not None and win1 != self.pyshell:
            win1.Destroy()
        
        if win2 is not None and win2 != self.pyshell:
            win2.Destroy()

        if python_code and atlas_cpp_code:
            self.__BuildBothCodeViewers(python_code, atlas_cpp_code)
        elif python_code:
            self.__BuildBothCodeViewers(python_code, '')
        elif atlas_cpp_code:
            self.__BuildBothCodeViewers('', atlas_cpp_code)

        self.Layout()

    def __BuildBothCodeViewers(self, python_code, atlas_cpp_code):
        vsplitter = wx.SplitterWindow(self.hsplitter, style=wx.SP_LIVE_UPDATE)
        python_textctrl = wx.TextCtrl(vsplitter, style=wx.TE_MULTILINE|wx.TE_READONLY)
        cpp_textctrl = wx.TextCtrl(vsplitter, style=wx.TE_MULTILINE|wx.TE_READONLY)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL)
        python_textctrl.SetFont(mono10)
        cpp_textctrl.SetFont(mono10)

        python_textctrl.SetValue(python_code)
        cpp_textctrl.SetValue(atlas_cpp_code)

        vsplitter.SplitVertically(python_textctrl, cpp_textctrl)
        self.hsplitter.SplitHorizontally(vsplitter, self.pyshell)
        self.Layout()

        if not python_code:
            vsplitter.Unsplit(python_textctrl)
        
        if not atlas_cpp_code:
            vsplitter.Unsplit(cpp_textctrl)
