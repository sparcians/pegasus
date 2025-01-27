import wx
from backend.sim_api import *
from backend.atlas_dtypes import *
from backend.observers import *
from backend.state_db import StateDB
from backend.state_serializer import StateSerializer
from backend.state_query import StateQuery

class InstViewer(wx.Panel):
    def __init__(self, parent, frame, test_debugger):
        wx.Panel.__init__(self, parent)
        self.SetBackgroundColour('green')
        self.frame = frame
        self.test_debugger = test_debugger

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.inst_list_ctrl = wx.ListCtrl(self, style=wx.LC_REPORT | wx.LC_SINGLE_SEL)
        self.inst_list_ctrl.SetFont(mono10)
        self.insts_by_pc = {}

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.inst_list_ctrl, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

        self.inst_list_ctrl.Bind(wx.EVT_LIST_ITEM_SELECTED, self.__OnInstViewerItemSelected)

    def LoadTest(self, test):
        self.inst_list_ctrl.ClearAll()
        self.inst_list_ctrl.InsertColumn(0, "PC")
        self.inst_list_ctrl.InsertColumn(1, "Disasm")
        self.inst_list_ctrl.InsertItem(0, "Initial State")

        riscv_tests_dir = self.frame.riscv_tests_dir
        sim_exe_path = self.frame.sim_exe_path
        obs_sim = ObserverSim(riscv_tests_dir, sim_exe_path, test)
        state_db = StateDB()
        obs = StateSerializer(state_db)
        state_db.CallInTransaction(lambda: obs_sim.RunObserver(obs, timeout=10))
        state_query = StateQuery(state_db.db_file.name)

        for pc, _, dasm, inst_uid in state_query.GetInstructions():
            i = self.inst_list_ctrl.GetItemCount()
            pc = '0x{:x}'.format(pc)
            self.inst_list_ctrl.InsertItem(i, pc)
            self.inst_list_ctrl.SetItem(i, 1, dasm)
            self.insts_by_pc[pc] = inst_uid

        if obs.infinite_loop_pc:
            i = self.inst_list_ctrl.GetItemCount()
            self.inst_list_ctrl.InsertItem(i, hex(obs.infinite_loop_pc))
            self.inst_list_ctrl.SetItem(i, 1, "Infinite loop detected")

        self.inst_list_ctrl.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.inst_list_ctrl.SetColumnWidth(1, wx.LIST_AUTOSIZE)

        # Set our size to be the minimum size needed to show all items
        colwidth0 = self.inst_list_ctrl.GetColumnWidth(0)
        colwidth1 = self.inst_list_ctrl.GetColumnWidth(1)
        minwidth = colwidth0 + colwidth1 + 10
        self.inst_list_ctrl.SetSize((minwidth, wx.DisplaySize()[1]))
        self.inst_list_ctrl.Select(0)

        self.Layout()

        return state_query

    def __OnInstViewerItemSelected(self, event):
        selected_item = event.GetIndex()
        text = self.inst_list_ctrl.GetItemText(selected_item)
        if text == 'Initial State':
            self.test_debugger.LoadInitialState()
        else:
            pc = text
            inst = self.insts_by_pc[pc]
            self.test_debugger.LoadInst(pc, inst)
