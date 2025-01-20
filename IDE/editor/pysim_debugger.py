import wx
import wx.py.shell
from backend.sim_wrapper import SimWrapper
from backend.dtypes import PySimRegister, ErrorResponse

class RegisterAccessor:
    def __init__(self, pysim_debugger):
        self.__pysim_debugger = pysim_debugger
        self.__known_regs_by_name = {}

    def __getattr__(self, reg_name):
        if reg_name in self.__known_regs_by_name:
            return self.__known_regs_by_name[reg_name]

        if reg_name in ('rs1', 'rs2', 'rd', 'imm'):
            reg_name = self.__pysim_debugger.PingAtlas('inst_reg {}'.format(reg_name))

        cmd = 'regprop {} group_num'.format(reg_name)
        group_num = self.__pysim_debugger.PingAtlas(cmd)
        if isinstance(group_num, ErrorResponse):
            return None

        cmd = 'regprop {} reg_id'.format(reg_name)
        reg_id = self.__pysim_debugger.PingAtlas(cmd)
        if isinstance(reg_id, ErrorResponse):
            return None

        assert group_num >= 0 and reg_id >= 0
        reg = PySimRegister(self.__pysim_debugger, reg_name, group_num, reg_id)

        if reg_name not in ('rs1', 'rs2', 'rd', 'imm'):
            self.__known_regs_by_name[reg_name] = reg

        return reg

class MiniDebugger:
    def __init__(self, pysim_debugger):
        self.__pysim_debugger = pysim_debugger
        self.__register_accessor = None

    @property
    def regs(self):
        if not self.__register_accessor:
            self.__register_accessor = RegisterAccessor(self.__pysim_debugger)

        return self.__register_accessor

    def BreakInst(self, mnemonic):
        self.__pysim_debugger.BreakInst(mnemonic)

    def DeleteBreakpoints(self):
        self.__pysim_debugger.DeleteBreakpoints()

    def ShowCode(self, language):
        self.__pysim_debugger.ShowCode(language)

    def ExecuteCode(self, language):
        self.__pysim_debugger.ExecuteCode(language)

    def Continue(self):
        self.__pysim_debugger.Continue()

class InstViewer(wx.Panel):
    def __init__(self, parent, code_mgr):
        wx.Panel.__init__(self, parent)
        self.code_mgr = code_mgr

        vsplitter = wx.SplitterWindow(self, style=wx.SP_LIVE_UPDATE)
        left_panel = wx.Panel(vsplitter)
        right_panel = wx.Panel(vsplitter)
        self.python_textctrl = wx.TextCtrl(left_panel, style=wx.TE_MULTILINE|wx.TE_READONLY)
        self.atlas_cpp_text = wx.StaticText(left_panel)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL)
        self.python_textctrl.SetFont(mono10)
        self.atlas_cpp_text.SetFont(mono10)

        self.left_panel_sizer = wx.BoxSizer(wx.VERTICAL)
        self.left_panel_sizer.Add(self.python_textctrl, 1, wx.EXPAND)
        self.left_panel_sizer.Add(self.atlas_cpp_text, 1, wx.EXPAND)
        left_panel.SetSizer(self.left_panel_sizer)

        # Inst info: PC, Mnemonic, Opcode, Privilege
        self.pc_subpanel = wx.Panel(right_panel)
        pc_label = wx.StaticText(self.pc_subpanel, label='PC:')
        self.pc_text = wx.StaticText(self.pc_subpanel)
        self.pc_text.SetFont(mono10)
        pc_label.SetFont(mono10)
        pc_subpanel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        pc_subpanel_sizer.Add(pc_label, 0, wx.EXPAND)
        pc_subpanel_sizer.Add(self.pc_text, 1, wx.EXPAND)
        self.pc_subpanel.SetSizer(pc_subpanel_sizer)

        self.mnemonic_subpanel = wx.Panel(right_panel)
        mnemonic_label = wx.StaticText(self.mnemonic_subpanel, label='Mnemonic:')
        self.mnemonic_text = wx.StaticText(self.mnemonic_subpanel)
        self.mnemonic_text.SetFont(mono10)
        mnemonic_label.SetFont(mono10)
        mnemonic_subpanel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        mnemonic_subpanel_sizer.Add(mnemonic_label, 0, wx.EXPAND)
        mnemonic_subpanel_sizer.Add(self.mnemonic_text, 1, wx.EXPAND)
        self.mnemonic_subpanel.SetSizer(mnemonic_subpanel_sizer)

        self.opc_subpanel = wx.Panel(right_panel)
        opc_label = wx.StaticText(self.opc_subpanel, label='Opcode:')
        self.opc_text = wx.StaticText(self.opc_subpanel)
        self.opc_text.SetFont(mono10)
        opc_label.SetFont(mono10)
        opc_subpanel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        opc_subpanel_sizer.Add(opc_label, 0, wx.EXPAND)
        opc_subpanel_sizer.Add(self.opc_text, 1, wx.EXPAND)
        self.opc_subpanel.SetSizer(opc_subpanel_sizer)

        self.priv_subpanel = wx.Panel(right_panel)
        priv_label = wx.StaticText(self.priv_subpanel, label='Priv:')
        self.priv_text = wx.StaticText(self.priv_subpanel)
        self.priv_text.SetFont(mono10)
        priv_label.SetFont(mono10)
        priv_subpanel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        priv_subpanel_sizer.Add(priv_label, 0, wx.EXPAND)
        priv_subpanel_sizer.Add(self.priv_text, 1, wx.EXPAND)
        self.priv_subpanel.SetSizer(priv_subpanel_sizer)

        self.inst_info_sizer = wx.BoxSizer(wx.VERTICAL)
        self.inst_info_sizer.Add(self.pc_subpanel, 0, wx.EXPAND)
        self.inst_info_sizer.Add(self.mnemonic_subpanel, 0, wx.EXPAND)
        self.inst_info_sizer.Add(self.opc_subpanel, 0, wx.EXPAND)
        self.inst_info_sizer.Add(self.priv_subpanel, 0, wx.EXPAND)

        # Register info: RS1, RS2, RD, IMM
        self.rs1_subpanel = wx.Panel(right_panel)
        rs1_label = wx.StaticText(self.rs1_subpanel, label='RS1:')
        self.rs1_text = wx.StaticText(self.rs1_subpanel)
        self.rs1_text.SetFont(mono10)
        rs1_label.SetFont(mono10)
        rs1_subpanel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        rs1_subpanel_sizer.Add(rs1_label, 0, wx.EXPAND)
        rs1_subpanel_sizer.Add(self.rs1_text, 1, wx.EXPAND)
        self.rs1_subpanel.SetSizer(rs1_subpanel_sizer)

        self.rs2_subpanel = wx.Panel(right_panel)
        rs2_label = wx.StaticText(self.rs2_subpanel, label='RS2:')
        self.rs2_text = wx.StaticText(self.rs2_subpanel)
        self.rs2_text.SetFont(mono10)
        rs2_label.SetFont(mono10)
        rs2_subpanel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        rs2_subpanel_sizer.Add(rs2_label, 0, wx.EXPAND)
        rs2_subpanel_sizer.Add(self.rs2_text, 1, wx.EXPAND)
        self.rs2_subpanel.SetSizer(rs2_subpanel_sizer)

        self.rd_subpanel = wx.Panel(right_panel)
        rd_label = wx.StaticText(self.rd_subpanel, label='RD:')
        self.rd_text = wx.StaticText(self.rd_subpanel)
        self.rd_text.SetFont(mono10)
        rd_label.SetFont(mono10)
        rd_subpanel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        rd_subpanel_sizer.Add(rd_label, 0, wx.EXPAND)
        rd_subpanel_sizer.Add(self.rd_text, 1, wx.EXPAND)
        self.rd_subpanel.SetSizer(rd_subpanel_sizer)

        self.imm_subpanel = wx.Panel(right_panel)
        imm_label = wx.StaticText(self.imm_subpanel, label='IMM:')
        self.imm_text = wx.StaticText(self.imm_subpanel)
        self.imm_text.SetFont(mono10)
        imm_label.SetFont(mono10)
        imm_subpanel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        imm_subpanel_sizer.Add(imm_label, 0, wx.EXPAND)
        imm_subpanel_sizer.Add(self.imm_text, 1, wx.EXPAND)
        self.imm_subpanel.SetSizer(imm_subpanel_sizer)

        self.reg_info_sizer = wx.BoxSizer(wx.VERTICAL)
        self.reg_info_sizer.Add(self.rs1_subpanel, 0, wx.EXPAND)
        self.reg_info_sizer.Add(self.rs2_subpanel, 0, wx.EXPAND)
        self.reg_info_sizer.Add(self.rd_subpanel, 0, wx.EXPAND)
        self.reg_info_sizer.Add(self.imm_subpanel, 0, wx.EXPAND)

        right_panel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        right_panel_sizer.Add(self.inst_info_sizer, 0, wx.EXPAND)
        right_panel_sizer.Add(self.reg_info_sizer, 0, wx.EXPAND)
        right_panel.SetSizer(right_panel_sizer)

        self.vsplitter = vsplitter
        self.left_panel = left_panel
        self.right_panel = right_panel
        self.vsplitter.SplitVertically(self.left_panel, self.right_panel)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(vsplitter, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def UpdateViewer(self, pc, inst, python_code, atlas_cpp_code):
        self.python_textctrl.SetValue(python_code)
        self.atlas_cpp_text.SetLabel(atlas_cpp_code)

        self.left_panel_sizer.Clear()
        if python_code:
            self.python_textctrl.Show()
            self.atlas_cpp_text.Hide()
            self.left_panel_sizer.Add(self.python_textctrl, 1, wx.EXPAND)
        else:
            self.python_textctrl.Hide()
            self.atlas_cpp_text.Show()
            self.left_panel_sizer.Add(self.atlas_cpp_text, 1, wx.EXPAND)

        # This info is always visible
        self.pc_text.SetLabel('0x'+hex(pc))
        self.mnemonic_text.SetLabel(inst.mnemonic)
        self.opc_text.SetLabel('0x'+hex(inst.opcode))

        priv_strs = {0:'U', 1:'S', 2:'H', 3:'M'}
        self.priv_text.SetLabel('{}({})'.format(priv_strs[inst.priv], inst.priv))

        # Some of this may not be there depending on the inst
        self.reg_info_sizer.Clear()
        self.rs1_subpanel.Hide()
        self.rs2_subpanel.Hide()
        self.rd_subpanel.Hide()
        self.imm_subpanel.Hide()

        # Add them back...
        if inst.rs1:
            self.rs1_text.SetLabel(inst.rs1)
            self.rs1_subpanel.Show()
            self.reg_info_sizer.Add(self.rs1_subpanel, 0, wx.EXPAND)

        if inst.rs2:
            self.rs2_text.SetLabel(inst.rs2)
            self.rs2_subpanel.Show()
            self.reg_info_sizer.Add(self.rs2_subpanel, 0, wx.EXPAND)

        if inst.rd:
            self.rd_text.SetLabel(inst.rd)
            self.rd_subpanel.Show()
            self.reg_info_sizer.Add(self.rd_subpanel, 0, wx.EXPAND)

        if inst.immediate:
            self.imm_text.SetLabel(str(inst.immediate))
            self.imm_subpanel.Show()
            self.reg_info_sizer.Add(self.imm_subpanel, 0, wx.EXPAND)

        # TODO cnyce: Fix this layout issue
        self.vsplitter.SetSashPosition(932)

        self.Layout()

    def ShowCode(self, language):
        self.left_panel_sizer.Clear()

        if language.lower() in ('python', 'py'):
            self.python_textctrl.Show()
            self.atlas_cpp_text.Hide()
            self.left_panel_sizer.Add(self.python_textctrl, 1, wx.EXPAND)
        elif language.lower() in ('cpp', 'c++'):
            self.python_textctrl.Hide()
            self.atlas_cpp_text.Show()
            self.left_panel_sizer.Add(self.atlas_cpp_text, 1, wx.EXPAND)
        else:
            raise ValueError("Invalid language: {}".format(language))

        self.Layout()

    def ExecuteCode(self, language):
        wx.MessageBox("TODO: Implement this feature")

class PySimDebugger(wx.Dialog):
    def __init__(self, workspace):
        wx.Dialog.__init__(self, None, title="PySim Debugger - {}".format(workspace.frame.GetTitle()))

        self.workspace = workspace
        self.riscv_tests_dir = workspace.frame.riscv_tests_dir
        self.sim_exe_path = workspace.frame.sim_exe_path
        self.test = workspace.frame.GetTitle()
        self.code_mgr = workspace.inst_editor.inst_impl_panel.runtime_code_mgr
        self.hsplitter = wx.SplitterWindow(self, style=wx.SP_LIVE_UPDATE)
        self.inst_viewer = InstViewer(self.hsplitter, self.code_mgr)
        self.pyshell = wx.py.shell.Shell(self.hsplitter, locals={'atlas': MiniDebugger(self)})

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
        self.pyshell.quit()
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
                self.__UpdateInstViewer(inst)
            elif action in ('no_response', 'sim_finished', ''):
                self.EndModal(wx.ID_OK)
        else:
            self.EndModal(wx.ID_OK)

    def ShowCode(self, language):
        self.inst_viewer.ShowCode(language)

    def PingAtlas(self, cmd):
        return self.sim_wrapper.PingAtlas(cmd)

    def __UpdateInstViewer(self, inst):
        python_code = self.code_mgr.GetPythonCode(inst.mnemonic)
        atlas_cpp_code = self.workspace.inst_editor.inst_impl_panel.GetAtlasCppCode(inst.mnemonic)
        pc = self.sim_wrapper.PingAtlas('pc')
        self.inst_viewer.UpdateViewer(pc, inst, python_code, atlas_cpp_code)
        self.hsplitter.SplitHorizontally(self.inst_viewer, self.pyshell)
