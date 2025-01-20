import wx

class RuntimeCodeManager:
    def __init__(self, frame, pyutils_textctrl, python_checkbox, cpp_checkbox):
        self.frame = frame
        self.pyutils_textctrl = pyutils_textctrl
        self.python_checkbox = python_checkbox
        self.cpp_checkbox = cpp_checkbox

        pyutils_textctrl.Bind(wx.EVT_TEXT, self.__OnTextChange)
        python_checkbox.Bind(wx.EVT_CHECKBOX, self.__OnCheckboxChange)
        cpp_checkbox.Bind(wx.EVT_CHECKBOX, self.__OnCheckboxChange)

        # Map from {mnemonic: (pycode, use_python_code)}
        self.revision_code_dict = {}

    def LoadInst(self, pc, inst):
        mnemonic = self.__GetCurrentInstMnemonic()

        if mnemonic in self.revision_code_dict:
            pycode, use_python_code = self.revision_code_dict[mnemonic]
            use_python_code = use_python_code and pycode != ''

            self.pyutils_textctrl.Enable()
            self.pyutils_textctrl.SetValue(pycode)

            if use_python_code:
                self.pyutils_textctrl.Enable()
                self.python_checkbox.SetValue(True)
                self.cpp_checkbox.SetValue(False)
            else:
                self.pyutils_textctrl.Disable()
                self.python_checkbox.SetValue(False)
                self.cpp_checkbox.SetValue(True)
        else:
            self.pyutils_textctrl.SetValue('')
            self.pyutils_textctrl.Disable()
            self.python_checkbox.SetValue(False)
            self.cpp_checkbox.SetValue(True)

    def __GetCurrentInstMnemonic(self):
        page_idx = self.frame.notebook.GetSelection()
        page = self.frame.notebook.GetPage(page_idx)
        row = page.inst_viewer.inst_list_ctrl.GetFirstSelected()
        item = page.inst_viewer.inst_list_ctrl.GetItem(row, 1)

        # Parse the mnemonic from the disassembly:
        #    addi a0, a0, 1
        #    ^^^^
        return item.GetText().lstrip().split()[0]

    def __OnTextChange(self, event):
        if not self.pyutils_textctrl.IsEnabled():
            return
        
        if not self.python_checkbox.IsChecked():
            return

        pycode = self.pyutils_textctrl.GetValue()
        if pycode == '':
            return

        mnemonic = self.__GetCurrentInstMnemonic()
        self.revision_code_dict[mnemonic] = (self.pyutils_textctrl.GetValue(), True)

    def __OnCheckboxChange(self, event):
        mnemonic = self.__GetCurrentInstMnemonic()
        checkbox = event.GetEventObject()
        switch_to_python = (checkbox == self.python_checkbox and checkbox.GetValue()) or (checkbox == self.cpp_checkbox and not checkbox.GetValue())
        self.revision_code_dict[mnemonic] = (self.pyutils_textctrl.GetValue(), switch_to_python)

        if switch_to_python:
            self.python_checkbox.SetValue(True)
            self.cpp_checkbox.SetValue(False)
            self.pyutils_textctrl.Enable()
        else:
            self.cpp_checkbox.SetValue(True)
            self.python_checkbox.SetValue(False)
            self.pyutils_textctrl.Disable()
