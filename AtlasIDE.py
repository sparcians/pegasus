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

        # Add "baz" to passing (demo)
        self.AppendItem(self.passing_root, "baz")

        # Add "foo" and "bar" to failing (demo)
        self.AppendItem(self.failing_root, "foo")
        self.AppendItem(self.failing_root, "bar")

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

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.initial_diffs_viewer = InitialDiffsViewer(self, wdb)
        self.SetSizer(self.sizer)

        # we should have the windows: Info, Registers, Atlas Code, Spike Code, and a python prompt.
        self.inst_info = TestViewer.InstInfo(self)

    def LoadTest(self, test_name):
        pass

    class InstInfo:
        def __init__(self, parent):
            #self.pc = wx.StaticText(parent, -1, "PC: 0x00000000")l
            pass

class TestResults(wx.Panel):
    def __init__(self, parent, wdb):
        wx.Panel.__init__(self, parent, -1)
        self.wdb = wdb

        self.help = wx.StaticText(self, -1, "Test Results")
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)

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

if __name__ == "__main__":
    app = wx.App()

    # Enable frame inspection
    #import wx.lib.inspection
    #wx.lib.inspection.InspectionTool().Show()

    frame = AtlasIDE(sys.argv[1])
    frame.Show()
    app.MainLoop()
