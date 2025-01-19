import wx, os
import wx.py.shell

class InstEditor(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.frame = frame

        editor_splitter = wx.SplitterWindow(self, style=wx.SP_LIVE_UPDATE)
        top_panel = wx.Panel(editor_splitter)
        bottom_panel = wx.Panel(editor_splitter)

        top_row1_panel = wx.Panel(top_panel)
        top_row2_panel = wx.Panel(top_panel)

        self.inst_info_panel = InstInfo(top_row1_panel)
        self.reg_info_panel = RegInfo(top_row1_panel)
        self.example_impl_panel = ExampleImpl(top_row1_panel)
        self.inst_impl_panel = InstImpl(top_row2_panel, frame)
        self.shell = wx.py.shell.Shell(bottom_panel)

        top_row1_sizer = wx.BoxSizer(wx.HORIZONTAL)
        top_row1_sizer.Add(self.inst_info_panel, 1, wx.EXPAND)
        top_row1_sizer.AddStretchSpacer()
        top_row1_sizer.Add(self.reg_info_panel, 1, wx.EXPAND)
        top_row1_sizer.AddStretchSpacer()
        top_row1_sizer.Add(self.example_impl_panel, 1, wx.EXPAND)
        top_row1_panel.SetSizer(top_row1_sizer)

        top_row2_sizer = wx.BoxSizer(wx.HORIZONTAL)
        top_row2_sizer.Add(self.inst_impl_panel, 1, wx.EXPAND)
        top_row2_panel.SetSizer(top_row2_sizer)

        top_panel_sizer = wx.BoxSizer(wx.VERTICAL)
        top_panel_sizer.Add(top_row1_panel, 0, wx.EXPAND)
        top_panel_sizer.Add(wx.StaticLine(top_panel), 0, wx.EXPAND)
        top_panel_sizer.Add(top_row2_panel, 1, wx.EXPAND)
        top_panel.SetSizer(top_panel_sizer)

        bottom_sizer = wx.BoxSizer(wx.VERTICAL)
        bottom_sizer.Add(self.shell, 1, wx.EXPAND)
        bottom_panel.SetSizer(bottom_sizer)

        editor_splitter.SplitHorizontally(top_panel, bottom_panel)
        editor_splitter.SetSashGravity(0.8)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(editor_splitter, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def LoadInst(self, pc, inst):
        self.inst_info_panel.LoadInst(pc, inst)
        self.reg_info_panel.LoadInst(pc, inst)
        self.example_impl_panel.LoadInst(pc, inst)
        self.inst_impl_panel.LoadInst(pc, inst)
        self.Layout()

class InstInfo(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.inst_info_text = wx.StaticText(self)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.inst_info_text.SetFont(mono10)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.inst_info_text, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def LoadInst(self, pc, inst):
        pc = hex(pc) if not isinstance(pc, str) else pc
        mnemonic = inst.mnemonic
        opcode = hex(inst.opcode) if not isinstance(inst.opcode, str) else inst.opcode
        priv = {0: 'U(0)', 1: 'S(1)', 2: 'H(2)', 3: 'M(3)'}[inst.priv]

        # PC:       <pc>
        # Mnemonic: <mnemonic>
        # Opcode:   <opcode>
        # Priv:     <priv>
        text = ['PC:'.ljust(10) + pc, 'Mnemonic:'.ljust(10) + mnemonic, 'Opcode:'.ljust(10) + right_justify_hex(opcode), 'Priv:'.ljust(10) + priv]
        self.inst_info_text.SetLabel('\n'.join(text))
        self.Layout()

class RegInfo(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.reg_info_text = wx.StaticText(self)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.reg_info_text.SetFont(mono10)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.reg_info_text, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def LoadInst(self, pc, inst):
        hex_width = 0

        lines = []
        if inst.rs1:
            col0 = 'RS1({}):'.format(inst.rs1)
            col1 = hex(inst.rs1val) if not isinstance(inst.rs1val, str) else inst.rs1val
            hex_width = max(hex_width, len(col1))
            lines.append([col0, col1])

        if inst.rs2:
            col0 = 'RS2({}):'.format(inst.rs2)
            col1 = hex(inst.rs2val) if not isinstance(inst.rs2val, str) else inst.rs2val
            hex_width = max(hex_width, len(col1))
            lines.append([col0, col1])

        if inst.rd:
            col0 = 'RD({}):'.format(inst.rd)
            col1 = hex(inst.rdval) if not isinstance(inst.rdval, str) else inst.rdval
            hex_width = max(hex_width, len(col1))
            lines.append([col0, col1])

        if inst.immediate:
            col0 = 'IMM:'
            col1 = hex(inst.immediate) if not isinstance(inst.immediate, str) else inst.immediate
            hex_width = max(hex_width, len(col1))
            lines.append([col0, col1])

        # Left justify the first column by the max length of the first column.
        # And right-jusitfy the second column accoring to right_justify_hex where
        # width is the smallest power of 2 that can contain the longest hex string.
        text = []
        for col0, col1 in lines:
            text.append(col0.ljust(10) + right_justify_hex(col1))

        self.reg_info_text.SetLabel('\n'.join(text))

class ExampleImpl(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.example_impl_text = wx.StaticText(self)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.example_impl_text.SetFont(mono10)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.example_impl_text, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def LoadInst(self, pc, inst):
        file_dir = os.path.dirname(__file__)
        atlas_root = os.path.abspath(os.path.join(file_dir, os.pardir, os.pardir))
        spike_root = os.path.join(atlas_root, 'spike')
        insns_root = os.path.join(spike_root, 'riscv', 'insns')
        impl_file = os.path.join(insns_root, inst.mnemonic + '.h')

        if os.path.isfile(impl_file):
            with open(impl_file, 'r') as fin:
                self.example_impl_text.SetLabel(fin.read())
        else:
            self.example_impl_text.SetLabel('')

class InstImpl(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)

        vsplitter = wx.SplitterWindow(self)

        pyutils_panel = wx.Panel(vsplitter)
        self.pyutils_textctrl = wx.TextCtrl(pyutils_panel, style=wx.TE_MULTILINE)
        self.pyutils_checkbox = wx.CheckBox(pyutils_panel, label='Use Python')
        self.runtime_code_mgr = RuntimeCodeManager(pyutils_panel, self.pyutils_textctrl, frame)
        self.pyutils_textctrl.Disable()

        # TODO cnyce: Fix the bugs in the revision mgr
        self.runtime_code_mgr.Disable()
        self.runtime_code_mgr.SetToolTip('This feature is not yet implemented')

        pyutils_row1_sizer = wx.BoxSizer(wx.HORIZONTAL)
        pyutils_row1_sizer.Add(self.pyutils_checkbox, 0, wx.EXPAND)
        pyutils_row1_sizer.AddStretchSpacer()
        pyutils_row1_sizer.Add(self.runtime_code_mgr, 1, wx.EXPAND)

        pyutils_vsizer = wx.BoxSizer(wx.VERTICAL)
        pyutils_vsizer.Add(pyutils_row1_sizer, 0, wx.EXPAND)
        pyutils_vsizer.Add(self.pyutils_textctrl, 1, wx.EXPAND)
        pyutils_panel.SetSizer(pyutils_vsizer)

        cpp_viewer_panel = wx.Panel(vsplitter)
        self.cpp_viewer_checkbox = wx.CheckBox(cpp_viewer_panel, label='Use C++')
        self.cpp_viewer_text = wx.StaticText(cpp_viewer_panel)
        self.cpp_viewer_checkbox.SetValue(True)

        cpp_viewer_vsizer = wx.BoxSizer(wx.VERTICAL)
        cpp_viewer_vsizer.Add(self.cpp_viewer_checkbox, 0, wx.EXPAND)
        cpp_viewer_vsizer.Add(wx.StaticLine(cpp_viewer_panel), 0, wx.EXPAND)
        cpp_viewer_vsizer.Add(self.cpp_viewer_text, 1, wx.EXPAND)
        cpp_viewer_panel.SetSizer(cpp_viewer_vsizer)

        vsplitter.SplitVertically(pyutils_panel, cpp_viewer_panel)
        vsplitter.SetMinimumPaneSize(20)
        vsplitter.SetSashPosition(500)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(vsplitter, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

        self.pyutils_checkbox.Bind(wx.EVT_CHECKBOX, self.__SwitchToPythonImpl)
        self.cpp_viewer_checkbox.Bind(wx.EVT_CHECKBOX, self.__SwitchToCppImpl)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        mono12 = wx.Font(12, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')

        self.pyutils_textctrl.SetFont(mono12)
        self.cpp_viewer_text.SetFont(mono10)

    def LoadInst(self, pc, inst):
        atlas_cpp_code = self.__GetAtlasCppCode(inst.mnemonic)
        self.cpp_viewer_text.SetLabel(atlas_cpp_code)
        self.pyutils_textctrl.SetValue('')

    def __SwitchToPythonImpl(self, event):
        self.pyutils_textctrl.Enable(self.pyutils_checkbox.IsChecked())
        self.cpp_viewer_checkbox.SetValue(not self.pyutils_checkbox.IsChecked())
        self.cpp_viewer_text.Disable()
        #self.runtime_code_mgr.Enable(self.pyutils_checkbox.IsChecked())

    def __SwitchToCppImpl(self, event):
        self.cpp_viewer_text.Enable(self.cpp_viewer_checkbox.IsChecked())
        self.pyutils_checkbox.SetValue(not self.cpp_viewer_checkbox.IsChecked())
        self.pyutils_textctrl.Disable()
        #self.runtime_code_mgr.Enable(self.pyutils_checkbox.IsChecked())

    def __GetAtlasCppCode(self, mnemonic, arch='rv64'):
        assert arch == 'rv64', 'rv32 has not been coded / tested yet'

        atlas_root = os.path.join(os.path.dirname(__file__), os.pardir, os.pardir)
        inst_handler_root = os.path.join(atlas_root, 'core', 'inst_handlers')
        inst_handler_root = os.path.abspath(inst_handler_root)
        lookfor = f"ActionGroup* {mnemonic}_64_handler(atlas::AtlasState* state);"

        # find the .hpp file under isnt_handler_root that has this <lookfor> string
        hpp_file = None
        for root, dirs, files in os.walk(inst_handler_root):
            for file in files:
                if file.endswith('.hpp'):
                    with open(os.path.join(root, file), 'r') as f:
                        if lookfor in f.read():
                            hpp_file = os.path.join(root, file)
                            break

        impl_file = hpp_file.replace('.hpp', '.cpp')

        cpp_code = []
        if os.path.exists(impl_file):
            lookfor = f"::{mnemonic}_64_handler(atlas::AtlasState* state)"
            other_mnemonic_lookfor = '_64_handler(atlas::AtlasState* state)'
            copy_line = False
            with open(impl_file, 'r') as f:
                for line in f.readlines():
                    # Stop copying lines when we get to the next function signature
                    if line.find(other_mnemonic_lookfor) != -1 and copy_line:
                        break

                    # Start copying over the code when we get to our exact function signature for this mnemonic
                    if line.find(lookfor) != -1:
                        copy_line = True

                    if copy_line:
                        cpp_code.append(line)

        # Remove 4 whitespaces from the front of each line
        cpp_code = [line[4:] for line in cpp_code]
        return ''.join(cpp_code)

class RuntimeCodeManager(wx.Panel):
    def __init__(self, parent, pyutils_textctrl, frame):
        wx.Panel.__init__(self, parent)
        self.frame = frame

        self.pyutils_textctrl = pyutils_textctrl
        self.add_button = wx.BitmapButton(self, bitmap=wx.ArtProvider.GetBitmap(wx.ART_PLUS))
        self.revisions_dropdown = wx.Choice(self, choices=['Revision 0'])
        self.revisions_dropdown.SetSelection(0)

        # Map from {mnemonic: [(rev_name, rev_code), ...]}
        self.revision_code_dict = {}

        self.add_button.Bind(wx.EVT_BUTTON, self.__OnAddRevision)
        self.revisions_dropdown.Bind(wx.EVT_CHOICE, self.__OnRevisionSelect)
        self.pyutils_textctrl.Bind(wx.EVT_TEXT, self.__OnTextChange)

        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.add_button, 0, wx.EXPAND)
        sizer.Add(self.revisions_dropdown, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def LoadInst(self, pc, inst):
        # Find all revision names for this inst and repopulate the dropdown
        inst_rev_names = []
        for name, code in self.revision_code_dict.get(inst.mnemonic, []):
            inst_rev_names.append(name)

        self.revisions_dropdown.Clear()
        self.revisions_dropdown.AppendItems(inst_rev_names)

        if len(inst_rev_names) == 0:
            self.revisions_dropdown.Append('Revision 0')

        self.revisions_dropdown.SetSelection(0)

    def __GetCurrentRevisionName(self):
        return self.revisions_dropdown.GetString(self.revisions_dropdown.GetSelection())

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
        rev_name = self.__GetCurrentRevisionName()
        mnemonic = self.__GetCurrentInstMnemonic()
        if mnemonic not in self.revision_code_dict:
            self.revision_code_dict[mnemonic] = []

        for i, (name, code) in enumerate(self.revision_code_dict[mnemonic]):
            if name == rev_name:
                self.revision_code_dict[mnemonic][i] = (name, self.pyutils_textctrl.GetValue())
                return

        self.revision_code_dict[mnemonic].append((rev_name, self.pyutils_textctrl.GetValue()))

    def __OnAddRevision(self, event):
        dlg = wx.TextEntryDialog(self, "Enter the revision name", "Revision Name")
        if dlg.ShowModal() == wx.ID_OK:
            self.revisions_dropdown.Append(dlg.GetValue())
            self.revisions_dropdown.SetSelection(self.revisions_dropdown.GetCount()-1)
            self.revision_code_dict[self.__GetCurrentInstMnemonic()].append((dlg.GetValue(), ''))
            self.pyutils_textctrl.SetValue('')
            self.Layout()

        dlg.Destroy()

    def __OnRevisionSelect(self, event):
        #revision_key = self.__GetRevisionKey()
        #revision_code = self.revision_code_dict.get(revision_key, '')
        #self.pyutils_textctrl.SetValue(revision_code)
        pass

def right_justify_hex(hex_str, width=8):
    # Remove the '0x' prefix and pad the hex string with leading zeros
    hex_value = hex_str[2:]  # Strip the "0x" prefix
    padded_hex = hex_value.zfill(width)  # Pad with zeros to the specified width
    return '0x' + padded_hex  # Re-add the "0x" prefix
