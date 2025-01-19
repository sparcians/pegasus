import wx
from editor.inst_editor import InstEditor
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
        self.inst_editor = InstEditor(self.workspace_panel, frame)
        self.inst_viewer = InstViewer(self.workspace_panel, frame, self)
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.inst_editor, 1, wx.EXPAND)
        sizer.Add(self.inst_viewer, 0, wx.EXPAND)
        self.workspace_panel.SetSizer(sizer)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.landing_page_panel, 1, wx.EXPAND)
        self.workspace_panel.Hide()
        self.SetSizer(sizer)
        self.Layout()

    def LoadTest(self, test):
        self.landing_page_panel.Hide()
        self.workspace_panel.Show()
        self.inst_viewer.LoadTest(test)
        self.GetSizer().Clear()
        self.GetSizer().Add(self.workspace_panel, 1, wx.EXPAND)
        self.Layout()
