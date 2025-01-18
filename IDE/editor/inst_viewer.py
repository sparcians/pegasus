import wx
from backend.sim_wrapper import SimWrapper

class InstViewer(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.SetBackgroundColour('green')
        self.frame = frame

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.inst_list_ctrl = wx.ListCtrl(self, style=wx.LC_REPORT | wx.LC_SINGLE_SEL)
        self.inst_list_ctrl.SetFont(mono10)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.inst_list_ctrl, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def LoadTest(self, test):
        self.inst_list_ctrl.ClearAll()
        self.inst_list_ctrl.InsertColumn(0, "PC")
        self.inst_list_ctrl.InsertColumn(1, "Disasm")

        with SimWrapper(self.frame.riscv_tests_dir, self.frame.sim_exe_path, test) as sim:
            init_regs = sim.GetRegisterValues()
            sim.BreakOnPostExecute()

            list_items = []
            while True:
                last_response = sim.Continue()
                assert isinstance(last_response, str)

                if last_response == 'post_execute':
                    inst = sim.GetCurrentInst()
                    if inst is None:
                        continue

                    pc = sim.GetPC()
                    dasm = inst.dasm
                    list_items.append((pc, dasm))
                elif last_response in ('no_response', 'sim_finished', ''):
                    break

            for i, (pc, dasm) in enumerate(list_items):
                self.inst_list_ctrl.InsertItem(i, hex(pc))
                self.inst_list_ctrl.SetItem(i, 1, dasm)

        self.inst_list_ctrl.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.inst_list_ctrl.SetColumnWidth(1, wx.LIST_AUTOSIZE)

        # Set our size to be the minimum size needed to show all items
        colwidth0 = self.inst_list_ctrl.GetColumnWidth(0)
        colwidth1 = self.inst_list_ctrl.GetColumnWidth(1)
        minwidth = colwidth0 + colwidth1 + 10
        self.inst_list_ctrl.SetSize((minwidth, wx.DisplaySize()[1]))

        self.Layout()
