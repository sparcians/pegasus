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
        rs1 = inst.rs1
        rs2 = inst.rs2
        rd  = inst.rd
        imm = inst.immediate
        self.reg_info_text.SetLabel('TODO: RegInfo')

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
        self.SetBackgroundColour('orange')
        self.edited_mnemonics = {}

    def LoadInst(self, pc, inst):
        pass
