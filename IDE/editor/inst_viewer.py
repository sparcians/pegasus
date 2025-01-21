import wx
from backend.sim_wrapper import SimWrapper
from editor.sim_api import *
from backend.dtypes import *

class InstViewer(wx.Panel):
    def __init__(self, parent, frame, workspace):
        wx.Panel.__init__(self, parent)
        self.SetBackgroundColour('green')
        self.frame = frame
        self.workspace = workspace
        self.inst_editor = workspace.inst_editor

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.inst_list_ctrl = wx.ListCtrl(self, style=wx.LC_REPORT | wx.LC_SINGLE_SEL)
        self.inst_list_ctrl.SetFont(mono10)
        self.insts_by_pc = {}

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.inst_list_ctrl, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def LoadTest(self, test):
        self.inst_list_ctrl.ClearAll()
        self.inst_list_ctrl.InsertColumn(0, "PC")
        self.inst_list_ctrl.InsertColumn(1, "Disasm")
        self.insts_by_pc = {}

        with SimWrapper(self.frame.riscv_tests_dir, self.frame.sim_exe_path, test) as sim:
            # We could get the instruction disassembly from pre- or post-execute but the
            # PC value can only be obtained during pre-execute. It will have advance to
            # the next PC by the time we get to post-execute.
            atlas_break_action(sim.endpoint, 'pre_execute')

            last_pc = -1
            while True:
                break_method = atlas_continue(sim.endpoint)
                if not atlas_sim_alive(sim.endpoint):
                    break

                pc = atlas_pc(sim.endpoint)
                if pc == last_pc:
                    break
                else:
                    last_pc = pc

                if break_method == 'pre_execute':
                    inst = atlas_current_inst(sim.endpoint)
                    if inst is None:
                        continue

                    i = self.inst_list_ctrl.GetItemCount()
                    self.inst_list_ctrl.InsertItem(i, hex(pc))
                    self.inst_list_ctrl.SetItem(i, 1, inst.dasmString())
                    self.insts_by_pc[hex(pc)] = inst.deepCopy()

        self.inst_list_ctrl.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.inst_list_ctrl.SetColumnWidth(1, wx.LIST_AUTOSIZE)

        # Set our size to be the minimum size needed to show all items
        colwidth0 = self.inst_list_ctrl.GetColumnWidth(0)
        colwidth1 = self.inst_list_ctrl.GetColumnWidth(1)
        minwidth = colwidth0 + colwidth1 + 10
        self.inst_list_ctrl.SetSize((minwidth, wx.DisplaySize()[1]))

        self.inst_list_ctrl.Bind(wx.EVT_LIST_ITEM_SELECTED, self.__OnItemSelected)
        self.Layout()

        # Select the first item
        self.inst_list_ctrl.SetItemState(0, wx.LIST_STATE_SELECTED, wx.LIST_STATE_SELECTED)

    def __OnItemSelected(self, event):
        row = event.GetIndex()
        pc = self.inst_list_ctrl.GetItemText(row, 0)
        if pc not in self.insts_by_pc:
            return

        inst = self.insts_by_pc[pc]
        self.inst_editor.LoadInst(pc, inst)
        self.workspace.inst_editor.inst_impl_panel.runtime_code_mgr.LoadInst(pc, inst)
