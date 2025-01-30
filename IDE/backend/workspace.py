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
        self.vsplitter = wx.SplitterWindow(self.workspace_panel, style=wx.SP_LIVE_UPDATE)
        self.test_debugger = TestDebugger(self.vsplitter, frame)
        self.inst_viewer = InstViewer(self.vsplitter, frame, self.test_debugger)
        self.vsplitter.SplitVertically(self.test_debugger, self.inst_viewer)
        self.vsplitter.SetMinimumPaneSize(50)
        self.vsplitter.SetSashPosition(1310)

        workspace_sizer = wx.BoxSizer(wx.HORIZONTAL)
        workspace_sizer.Add(self.vsplitter, 1, wx.EXPAND)
        self.workspace_panel.SetSizer(workspace_sizer)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.landing_page_panel, 1, wx.EXPAND)
        self.workspace_panel.Hide()
        self.SetSizer(sizer)
        self.Layout()

    def LoadTest(self, test, arch):
        self.landing_page_panel.Hide()
        self.workspace_panel.Show()

        self.GetSizer().Clear()
        self.GetSizer().Add(self.workspace_panel, 1, wx.EXPAND)
        self.frame.SetTitle(test)

        state_query = self.inst_viewer.LoadTest(test)
        self.test_debugger.SetStateQuery(state_query)
        self.test_debugger.SetArch(arch)

        # Workaround to force the layout to occur
        sash_pos = self.vsplitter.GetSashPosition()
        self.vsplitter.SetSashPosition(sash_pos-1)
        self.vsplitter.SetSashPosition(sash_pos)
        self.vsplitter.UpdateSize()
