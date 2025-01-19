import wx
from functools import partial

SUPPORTED_EXTENSIONS = ["mi", "si", "ui", "um", "ua", "uf", "ud"]
SUPPORTED_ARCHES = ["rv32", "rv64"]

class TestConfig(wx.Dialog):
    def __init__(self, frame):
        wx.Dialog.__init__(self, frame)
        self.frame = frame

        active_workspace = self.frame.notebook.GetPage(self.frame.notebook.GetSelection())
        active_workspace_name = self.frame.notebook.GetPageText(self.frame.notebook.GetSelection())
        self.SetTitle(f"Test Config: {active_workspace_name}")

        # The dialog will show the following test selections (checkboxes)
        #
        #   [loaded test]
        #   [mi] [si] [ui] [um] [ua] [uf] [ud]
        #   [rv32] [rv64]
        self.test_selection_checkboxes = []
        vsizer = wx.BoxSizer(wx.VERTICAL)

        loaded_test = frame.GetTitle()
        if loaded_test and (loaded_test.find('rv32') == 0 or loaded_test.find('rv64') == 0) and loaded_test.find('-p-') != -1:
            checkbox = wx.CheckBox(self, label=loaded_test)
            checkbox.SetValue(True)
            vsizer.Add(checkbox)

        row_sizer = wx.BoxSizer(wx.HORIZONTAL)
        for ext in SUPPORTED_EXTENSIONS:
            checkbox = wx.CheckBox(self, label=ext)
            row_sizer.Add(checkbox)
            self.test_selection_checkboxes.append(checkbox)

        vsizer.Add(row_sizer)

        row_sizer = wx.BoxSizer(wx.HORIZONTAL)
        for arch in SUPPORTED_ARCHES:
            checkbox = wx.CheckBox(self, label=arch)
            checkbox.SetValue(arch=='rv64')
            row_sizer.Add(checkbox)
            self.test_selection_checkboxes.append(checkbox)

        vsizer.Add(row_sizer)
        vsizer.Add(wx.StaticLine(self), 0, wx.EXPAND)

        run_btn = wx.Button(self, label="Run")
        run_btn.Bind(wx.EVT_BUTTON, partial(self.__EndModal, code=wx.ID_OK))

        cancel_btn = wx.Button(self, label="Cancel")
        cancel_btn.Bind(wx.EVT_BUTTON, partial(self.__EndModal, code=wx.ID_CANCEL))

        btn_sizer = wx.BoxSizer(wx.HORIZONTAL)
        btn_sizer.Add(run_btn)
        btn_sizer.Add(cancel_btn)
        vsizer.Add(btn_sizer, 0, wx.ALIGN_RIGHT)

        vsizer.Fit(self)
        self.SetSizer(vsizer)
        self.Layout()

    def __EndModal(self, event, code):
        self.EndModal(code)
