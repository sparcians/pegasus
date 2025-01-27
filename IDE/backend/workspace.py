import wx
from editor.test_debugger import TestDebugger
from editor.inst_viewer import InstViewer

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
        vsplitter = wx.SplitterWindow(self.workspace_panel, style=wx.SP_LIVE_UPDATE)
        self.test_debugger = TestDebugger(vsplitter, frame)
        self.inst_viewer = InstViewer(vsplitter, frame, self.test_debugger)
        vsplitter.SplitVertically(self.test_debugger, self.inst_viewer)
        vsplitter.SetMinimumPaneSize(50)
        vsplitter.SetSashPosition(1310)

        def PrintSashPos(event):
            print(vsplitter.GetSashPosition())

        vsplitter.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGED, PrintSashPos)

        workspace_sizer = wx.BoxSizer(wx.HORIZONTAL)
        workspace_sizer.Add(vsplitter, 1, wx.EXPAND)
        self.workspace_panel.SetSizer(workspace_sizer)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.landing_page_panel, 1, wx.EXPAND)
        self.workspace_panel.Hide()
        self.SetSizer(sizer)
        self.Layout()

    def LoadTest(self, test):
        self.landing_page_panel.Hide()
        self.workspace_panel.Show()
        self.frame.SetTitle(test)

        state_query = self.inst_viewer.LoadTest(test)
        self.test_debugger.SetStateQuery(state_query)

        self.GetSizer().Clear()
        self.GetSizer().Add(self.workspace_panel, 1, wx.EXPAND)
        self.Layout()
