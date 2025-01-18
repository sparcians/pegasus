import wx, os

class InstEditor(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.frame = frame

        row1_panel = wx.Panel(self)
        self.inst_info_panel = InstInfo(row1_panel)
        self.reg_info_panel = RegInfo(row1_panel)
        self.example_impl_panel = ExampleImpl(row1_panel)

        row1_sizer = wx.BoxSizer(wx.HORIZONTAL)
        row1_sizer.Add(self.inst_info_panel, 1, wx.EXPAND)
        row1_sizer.Add(self.reg_info_panel, 1, wx.EXPAND)
        row1_sizer.Add(self.example_impl_panel, 1, wx.EXPAND)
        row1_panel.SetSizer(row1_sizer)

        row2_panel = wx.Panel(self)
        self.inst_impl_panel = InstImpl(row2_panel)

        row2_sizer = wx.BoxSizer(wx.HORIZONTAL)
        row2_sizer.Add(self.inst_impl_panel, 1, wx.EXPAND)
        row2_panel.SetSizer(row2_sizer)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(row1_panel, 1, wx.EXPAND)
        sizer.Add(wx.StaticLine(self), 0, wx.EXPAND)
        sizer.Add(row2_panel, 1, wx.EXPAND)
        self.SetSizer(sizer)

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
        text = ['PC:'.ljust(10) + pc, 'Mnemonic:'.ljust(10) + mnemonic, 'Opcode:'.ljust(10) + opcode, 'Priv:'.ljust(10) + priv]
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
        def right_justify_hex(hex_str, width=8):
            # Remove the '0x' prefix and pad the hex string with leading zeros
            hex_value = hex_str[2:]  # Strip the "0x" prefix
            padded_hex = hex_value.zfill(width)  # Pad with zeros to the specified width
            return '0x' + padded_hex  # Re-add the "0x" prefix

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
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)

        vsplitter = wx.SplitterWindow(self)

        pyutils_panel = wx.Panel(vsplitter)
        self.pyutils_checkbox = wx.CheckBox(pyutils_panel, label='Use Python')
        self.pyutils_textctrl = wx.TextCtrl(pyutils_panel, style=wx.TE_MULTILINE)

        pyutils_vsizer = wx.BoxSizer(wx.VERTICAL)
        pyutils_vsizer.Add(self.pyutils_checkbox, 0, wx.EXPAND)
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

    def __SwitchToCppImpl(self, event):
        self.cpp_viewer_text.Enable(self.cpp_viewer_checkbox.IsChecked())
        self.pyutils_checkbox.SetValue(not self.cpp_viewer_checkbox.IsChecked())
        self.pyutils_textctrl.Disable()

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
