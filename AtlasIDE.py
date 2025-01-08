import os, sys, wx, sqlite3

class AtlasIDE(wx.Frame):
    def __init__(self, wdb_file):
        wx.Frame.__init__(self, None, -1, "Atlas IDE", size=wx.DisplaySize())
        self.wdb = WorkloadsDB(wdb_file)

        # Create a notebook with two tabs: "Test Debugger" and "Test Results"
        self.notebook = wx.Notebook(self, -1, style=wx.NB_TOP)
        self.test_debugger = TestDebugger(self.notebook, self.wdb)
        self.test_results = TestResults(self.notebook, self.wdb)
        self.notebook.AddPage(self.test_debugger, "Test Debugger")
        self.notebook.AddPage(self.test_results, "Test Results")

class TestDebugger(wx.Panel):
    def __init__(self, parent, wdb):
        wx.Panel.__init__(self, parent, -1)
        self.wdb = wdb

        self.test_selector = TestSelector(self, wdb)
        self.test_viewer = TestViewer(self, wdb)

        self.sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer.Add(self.test_selector, 0, wx.EXPAND)
        self.sizer.Add(wx.StaticLine(self, -1, style=wx.LI_VERTICAL), 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 5)
        self.sizer.Add(self.test_viewer, 1, wx.EXPAND)
        self.SetSizer(self.sizer)

    def LoadTest(self, test_name):
        self.test_viewer.LoadTest(test_name)

class TestSelector(wx.TreeCtrl):
    def __init__(self, parent, wdb):
        wx.TreeCtrl.__init__(self, parent, -1, style=wx.TR_HIDE_ROOT|wx.TR_HAS_BUTTONS|wx.NO_BORDER, size=(240, parent.GetSize().GetHeight()))
        self.wdb = wdb

        self.test_debugger = parent
        self.root = self.AddRoot("Root")
        self.passing_root = self.AppendItem(self.root, "Passing Tests")
        self.failing_root = self.AppendItem(self.root, "Failing Tests")

        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.__OnSelChanged)

        for test_name in self.wdb.GetPassingTestNames():
            self.AppendItem(self.passing_root, test_name)

        for test_name in self.wdb.GetFailingTestNames():
            self.AppendItem(self.failing_root, test_name)

        self.Collapse(self.passing_root)
        self.Expand(self.failing_root)

    def __OnSelChanged(self, evt):
        item = evt.GetItem()
        if item and item.IsOk():
            if item == self.passing_root or item == self.failing_root:
                return

            test = self.GetItemText(item)
            self.test_debugger.LoadTest(test)

class TestViewer(wx.Panel):
    def __init__(self, parent, wdb):
        wx.Panel.__init__(self, parent, -1)
        self.wdb = wdb

        self.initial_diffs_viewer = InitialDiffsViewer(self, wdb)
        self.inst_viewer = InstructionViewer(self, wdb)
        self.initial_diffs_viewer.Hide()

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.inst_viewer, 0, wx.EXPAND)
        self.SetSizer(self.sizer)

    def LoadTest(self, test_name):
        pass

class TestResults(wx.Panel):
    def __init__(self, parent, wdb):
        wx.Panel.__init__(self, parent, -1)
        self.wdb = wdb

        self.help = wx.StaticText(self, -1, "Test Results")
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)

class InitialDiffsViewer(wx.Panel):
    def __init__(self, parent, wdb):
        wx.Panel.__init__(self, parent, -1)
        self.wdb = wdb
        self.grid = None

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self.sizer)

class InstructionViewer(wx.Panel):
    def __init__(self, parent, wdb):
        wx.Panel.__init__(self, parent, -1)
        self.wdb = wdb

        self.inst_info_panel = InstructionInfoPanel(self)
        self.register_info_panel = RegisterInfoPanel(self)
        self.inst_list_panel = InstructionListPanel(self)
        self.spike_code_panel = SpikeCodePanel(self)
        self.atlas_experimental_code_panel = AtlasExperimentalCodePanel(self)
        self.atlas_cpp_code_panel = AtlasCppCodePanel(self)
        self.python_terminal = PythonTerminal(self)

        row1_sizer = wx.BoxSizer(wx.HORIZONTAL)
        row1_sizer.Add(self.inst_info_panel, 0, wx.EXPAND)
        row1_sizer.Add(self.register_info_panel, 0, wx.EXPAND)
        row1_sizer.Add(self.spike_code_panel, 0, wx.EXPAND)

        row2_sizer = wx.BoxSizer(wx.HORIZONTAL)
        row2_sizer.Add(self.atlas_experimental_code_panel, 0, wx.EXPAND)
        row2_sizer.AddStretchSpacer(1)
        row2_sizer.Add(self.atlas_cpp_code_panel, 0, wx.EXPAND)

        gridlike_sizer = wx.BoxSizer(wx.VERTICAL)
        gridlike_sizer.Add(row1_sizer, 0, wx.EXPAND)
        gridlike_sizer.AddSpacer(30)
        gridlike_sizer.Add(row2_sizer, 0, wx.EXPAND)

        row1_sizer = wx.BoxSizer(wx.HORIZONTAL)
        row1_sizer.Add(gridlike_sizer, 0, wx.EXPAND)
        row1_sizer.AddStretchSpacer(1)
        row1_sizer.Add(self.inst_list_panel, 0, wx.EXPAND)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(row1_sizer, 1, wx.EXPAND)
        self.sizer.AddStretchSpacer(1)
        self.sizer.Add(self.python_terminal, 0, wx.EXPAND)

        self.SetSizer(self.sizer)
        self.Layout()

class InstructionInfoPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Instruction Info")
        self.help.SetFont(mono12bold)

        self.pc_label = wx.StaticText(self, -1, "PC:")
        self.pc_label.SetFont(mono10)

        self.pc = wx.StaticText(self, -1, "0x--------")
        self.pc.SetFont(mono10)

        self.mnemonic_label = wx.StaticText(self, -1, "Inst:")
        self.mnemonic_label.SetFont(mono10)

        self.mnemonic = wx.StaticText(self, -1, "----")
        self.mnemonic.SetFont(mono10)

        self.opcode_label = wx.StaticText(self, -1, "Opcode:")
        self.opcode_label.SetFont(mono10)

        self.opcode = wx.StaticText(self, -1, "0x--------")
        self.opcode.SetFont(mono10)

        self.priv_label = wx.StaticText(self, -1, "Priv:")
        self.priv_label.SetFont(mono10)

        self.priv = wx.StaticText(self, -1, "-")
        self.priv.SetFont(mono10)

        self.result_label = wx.StaticText(self, -1, "Result:")
        self.result_label.SetFont(mono10)

        self.result = wx.StaticText(self, -1, "----")
        self.result.SetFont(mono10)

        # Use a grid sizer to lay out the instruction fields
        gridsizer = wx.FlexGridSizer(0, 2, 5, 5)
        gridsizer.AddGrowableCol(1)
        gridsizer.Add(self.pc_label, 0, wx.ALIGN_LEFT)
        gridsizer.Add(self.pc, 1, wx.EXPAND)
        gridsizer.Add(self.mnemonic_label, 0, wx.ALIGN_LEFT)
        gridsizer.Add(self.mnemonic, 1, wx.EXPAND)
        gridsizer.Add(self.opcode_label, 0, wx.ALIGN_LEFT)
        gridsizer.Add(self.opcode, 1, wx.EXPAND)
        gridsizer.Add(self.priv_label, 0, wx.ALIGN_LEFT)
        gridsizer.Add(self.priv, 1, wx.EXPAND)
        gridsizer.Add(self.result_label, 0, wx.ALIGN_LEFT)
        gridsizer.Add(self.result, 1, wx.EXPAND)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.sizer.Add(gridsizer, 0, wx.EXPAND)

        self.SetSizer(self.sizer)
        self.SetMinSize((400, -1))
        self.Layout()

class RegisterInfoPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Register Info")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((400, -1))
        self.Layout()

class InstructionListPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Instruction List")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((400, -1))
        self.Layout()

class SpikeCodePanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Spike Code")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((400, -1))
        self.Layout()

class AtlasExperimentalCodePanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Atlas Experimental Code")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((450, -1))
        self.Layout()

class AtlasCppCodePanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Atlas C++ Code")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((450, -1))
        self.Layout()

class PythonTerminal(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Python Terminal")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((wx.DisplaySize()[0], 40))
        self.Layout()

class WorkloadsDB:
    def __init__(self, wdb_file):
        self.conn = sqlite3.connect(wdb_file)
        self.cursor = self.conn.cursor()

        self.cursor.execute('SELECT Id,TestName FROM RiscvTests')
        self.test_names_by_id = dict(self.cursor.fetchall())
        self.test_ids_by_name = dict((v,k) for k,v in self.test_names_by_id.items())

        self.cursor.execute('SELECT DISTINCT(TestId) FROM InitValDiffs')
        self.failing_test_ids = {row[0] for row in self.cursor.fetchall()}

        self.cursor.execute('SELECT TestId,ResultCode FROM Instructions')
        for test_id, result_code in self.cursor.fetchall():
            # 2: pc invalid
            # 3: reg val invalid
            # 4: unimplemented inst
            if result_code >> 16 in (2,3,4):
                self.failing_test_ids.add(test_id)

    def GetTestId(self, test_name):
        return self.test_ids_by_name[test_name]

    def GetTestName(self, test_id):
        return self.test_names_by_id[test_id]

    def IsFailing(self, test_id):
        if isinstance(test_id, str):
            test_id = self.GetTestId(test_id)
        return test_id in self.failing_test_ids

    def GetPassingTestNames(self):
        test_names = [test_name for test_id, test_name in self.test_names_by_id.items() if not self.IsFailing(test_id)]
        test_names.sort()
        return test_names

    def GetFailingTestNames(self):
        test_names = [test_name for test_id, test_name in self.test_names_by_id.items() if self.IsFailing(test_id)]
        test_names.sort()
        return test_names

if __name__ == "__main__":
    app = wx.App()

    # Enable frame inspection
    #import wx.lib.inspection
    #wx.lib.inspection.InspectionTool().Show()

    frame = AtlasIDE(sys.argv[1])
    frame.Show()
    app.MainLoop()
