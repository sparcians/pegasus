import os, sys, wx, sqlite3
import wx.grid
from enum import IntEnum

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

class AtlasPanel(wx.Panel):
    def __init__(self, *args, **kwargs):
        wx.Panel.__init__(self, *args, **kwargs)
        self.frame = self.GetTopLevelParent()

class TestDebugger(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
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

class TestViewer(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
        self.wdb = wdb

        self.initial_diffs_viewer = InitialDiffsViewer(self, wdb)
        self.inst_viewer = InstructionViewer(self, wdb)

        self.initial_diffs_viewer.Hide()
        self.inst_viewer.Hide()

        self.inst_list_panel = InstructionListPanel(self)
        self.python_terminal = PythonTerminal(self)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self.sizer)

    def LoadTest(self, test_name):
        self.frame.SetTitle(test_name)
        self.initial_diffs_viewer.OnLoadTest(test_name)
        self.inst_viewer.OnLoadTest(test_name)
        self.python_terminal.OnLoadTest(test_name)
        self.inst_list_panel.OnLoadTest(test_name)

    def ShowInitialState(self):
        self.sizer.Clear()
        self.initial_diffs_viewer.Show()
        self.inst_viewer.Hide()

        row1_sizer = wx.BoxSizer(wx.HORIZONTAL)
        row1_sizer.Add(self.initial_diffs_viewer, 1, wx.EXPAND)
        row1_sizer.AddStretchSpacer(1)
        row1_sizer.Add(self.inst_list_panel, 0, wx.EXPAND)

        self.sizer.Add(row1_sizer, 1, wx.EXPAND)
        self.sizer.AddStretchSpacer(1)
        self.sizer.Add(wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL), 0, wx.EXPAND)
        self.sizer.Add(self.python_terminal, 0, wx.EXPAND)

        self.Layout()

    def ShowInstruction(self, pc):
        self.sizer.Clear()
        self.initial_diffs_viewer.Hide()
        self.inst_viewer.Show()

        row1_sizer = wx.BoxSizer(wx.HORIZONTAL)
        row1_sizer.Add(self.inst_viewer, 1, wx.EXPAND)
        row1_sizer.AddStretchSpacer(1)
        row1_sizer.Add(self.inst_list_panel, 0, wx.EXPAND)

        self.sizer.Add(row1_sizer, 1, wx.EXPAND)
        self.sizer.AddStretchSpacer(1)
        self.sizer.Add(self.python_terminal, 0, wx.EXPAND)

        self.inst_viewer.ShowInstruction(pc)
        self.Layout()

class TestResults(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
        self.wdb = wdb

        self.help = wx.StaticText(self, -1, "Test Results")
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)

class InitialDiffsViewer(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
        self.wdb = wdb
        self.grid = None

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self.sizer)

    def OnLoadTest(self, test_name):
        if self.grid:
            self.sizer.Clear()
            self.grid.Destroy()
            self.grid = None

        self.grid = wx.grid.Grid(self, -1)
        test_id = self.wdb.GetTestId(test_name)

        # Algo:
        #  1. Go through all the appropriate rows in the Registers table.
        #  2. For each register, get its initial value and expected value.
        #  3. Append the row info and use the "actual!=expected" test to set the background color.

        # diffs.append({'group_num': group_num, 'reg_idx': reg_idx, 'expected_val': expected_val, 'actual_val': actual_val})
        init_reg_diffs = self.wdb.GetInitialDiffs(test_id)

        # reg_vals.append({'reg_type': reg_type, 'reg_idx': reg_idx, 'init_val': init_val})
        init_reg_vals = self.wdb.GetInitialRegVals(test_id)

        row_content = []
        for reg_val in init_reg_vals:
            group_num = reg_val['reg_type']
            reg_idx = reg_val['reg_idx']
            actual_val = reg_val['init_val']
            expected_val = actual_val

            for diff in init_reg_diffs:
                if diff['group_num'] == group_num and diff['reg_idx'] == reg_idx:
                    expected_val = diff['expected_val']
                    break

            reg_name = self.wdb.GetRegisterName(group_num, reg_idx)
            row_content.append({'reg_name': reg_name, 'expected_val': expected_val, 'actual_val': actual_val})

        self.grid.CreateGrid(len(row_content), 3)
        self.grid.SetColLabelValue(0, "Register")
        self.grid.SetColLabelValue(1, "Expected Value")
        self.grid.SetColLabelValue(2, "Actual Value")

        for row, content in enumerate(row_content):
            self.grid.SetCellValue(row, 0, content['reg_name'])
            self.grid.SetCellValue(row, 1, hex(content['expected_val'] & 0xFFFFFFFFFFFFFFFF))
            self.grid.SetCellValue(row, 2, hex(content['actual_val'] & 0xFFFFFFFFFFFFFFFF))

            if content['expected_val'] != content['actual_val']:
                for col in range(3):
                    self.grid.SetCellBackgroundColour(row, col, 'pink')

        self.sizer.Add(self.grid, 1, wx.EXPAND)
        self.Layout()
        self.grid.AutoSize()

class InstructionViewer(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
        self.wdb = wdb

        self.inst_info_panel = InstructionInfoPanel(self)
        self.register_info_panel = RegisterInfoPanel(self)
        self.spike_code_panel = SpikeCodePanel(self)
        self.atlas_experimental_code_panel = AtlasExperimentalCodePanel(self)
        self.atlas_cpp_code_panel = AtlasCppCodePanel(self)

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

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(row1_sizer, 1, wx.EXPAND)

        self.SetSizer(self.sizer)
        self.Layout()

    def OnLoadTest(self, test_name):
        self.inst_info_panel.OnLoadTest(test_name)
        self.register_info_panel.OnLoadTest(test_name)
        self.spike_code_panel.OnLoadTest(test_name)
        self.atlas_experimental_code_panel.OnLoadTest(test_name)
        self.atlas_cpp_code_panel.OnLoadTest(test_name)

    def ShowInstruction(self, pc):
        self.inst_info_panel.ShowInstruction(pc)
        self.register_info_panel.ShowInstruction(pc)
        self.spike_code_panel.ShowInstruction(pc)
        self.atlas_experimental_code_panel.ShowInstruction(pc)
        self.atlas_cpp_code_panel.ShowInstruction(pc)

class InstructionInfoPanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Instruction Info")
        self.help.SetFont(mono12bold)

        self.pc_label = wx.StaticText(self, -1, "PC:")
        self.pc_label.SetFont(mono10)

        self.pc = wx.StaticText(self, -1, "")
        self.pc.SetFont(mono10)

        self.mnemonic_label = wx.StaticText(self, -1, "Inst:")
        self.mnemonic_label.SetFont(mono10)

        self.mnemonic = wx.StaticText(self, -1, "")
        self.mnemonic.SetFont(mono10)

        self.opcode_label = wx.StaticText(self, -1, "Opcode:")
        self.opcode_label.SetFont(mono10)

        self.opcode = wx.StaticText(self, -1, "")
        self.opcode.SetFont(mono10)

        self.priv_label = wx.StaticText(self, -1, "Priv:")
        self.priv_label.SetFont(mono10)

        self.priv = wx.StaticText(self, -1, "")
        self.priv.SetFont(mono10)

        self.result_label = wx.StaticText(self, -1, "Result:")
        self.result_label.SetFont(mono10)

        self.result = wx.StaticText(self, -1, "")
        self.result.SetFont(mono10)

        self.__ClearInfoPanel()

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

    def OnLoadTest(self, test_name):
        self.__ClearInfoPanel()
        self.test_id = self.frame.wdb.GetTestId(test_name)

    def ShowInstruction(self, pc):
        cmd = 'SELECT Mnemonic,Opcode,Priv,ResultCode FROM Instructions WHERE PC={} AND TestId={} AND HartId=0'.format(pc, self.test_id)
        self.frame.wdb.cursor.execute(cmd)
        mnemonic, opcode, priv, result_code = self.frame.wdb.cursor.fetchone()

        self.pc.SetLabel("0x{:08X}".format(pc))
        self.mnemonic.SetLabel(mnemonic)
        self.opcode.SetLabel("0x{:08X}".format(opcode))
        self.priv.SetLabel(["U({})", "S({})", "H({})", "M({})", "VU({})", "VS({})"][priv].format(priv))

        if result_code == 0:
            self.result.SetLabel('OKAY')
        elif result_code >> 16 == 0x1:
            exception_cause = result_code & 0xFFFF

            cause_strs = [
                'MISSALIGNED_FETCH',
                'FETCH_ACCESS',
                'ILLEGAL_INSTRUCTION',
                'BREAKPOINT',
                'MISSALIGNED_LOAD',
                'LOAD_ACCESS',
                'MISSALIGNED_STORE',
                'STORE_ACCESS',
                'USER_ECALL',
                'SUPERVISOR_ECALL',
                'VIRTUAL_SUPERVISOR_ECALL',
                'MACHINE_ECALL',
                'FETCH_PAGE_FAULT',
                'LOAD_PAGE_FAULT',
                'STORE_PAGE_FAULT',
                'DOUBLE_TRAP',
                'SOFTWARE_CHECK_FAULT',
                'HARDWARE_ERROR_FAULT',
                'FETCH_GUEST_PAGE_FAULT',
                'LOAD_GUEST_PAGE_FAULT',
                'VIRTUAL_INSTRUCTION',
                'STORE_GUEST_PAGE_FAULT'
            ]

            self.result.SetLabel(cause_strs[exception_cause])
        elif result_code >> 16 == 0x2:
            self.result.SetLabel('PC INVALID')
        elif result_code >> 16 == 0x3:
            self.result.SetLabel('REG VAL INVALID')
        elif result_code >> 16 == 0x4:
            self.result.SetLabel('UNIMPLEMENTED')

        self.Update()
        self.Layout()

    def __ClearInfoPanel(self):
        self.pc.SetLabel("")
        self.mnemonic.SetLabel("")
        self.opcode.SetLabel("")
        self.priv.SetLabel("")
        self.result.SetLabel("")

class RegisterInfoPanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Register Info")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((400, -1))
        self.Layout()

    def OnLoadTest(self, test_name):
        # TODO cnyce
        pass

    def ShowInstruction(self, pc):
        self.help.SetLabel('GOTO {}'.format(pc))
        pass

class InstructionListPanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Instruction List")
        self.help.SetFont(mono12bold)

        self.inst_list_ctrl = wx.ListCtrl(self, -1, style=wx.LC_REPORT|wx.LC_SINGLE_SEL|wx.LC_NO_HEADER|wx.LC_HRULES)
        self.inst_list_ctrl.Bind(wx.EVT_LIST_ITEM_SELECTED, self.__OnItemSelected)
        self.inst_list_ctrl.InsertColumn(0, "Instruction", width=200)
        self.inst_list_ctrl.SetFont(mono10)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.sizer.Add(self.inst_list_ctrl, 1, wx.EXPAND)
        self.SetSizer(self.sizer)

        self.SetMinSize((-1, self.frame.GetSize().GetHeight()))
        self.Layout()

    def OnLoadTest(self, test_name, hart_id=0):
        # Clear the ListCtrl first.
        self.inst_list_ctrl.DeleteAllItems()

        # Create a ListCtrl with two columns. The first column holds a bitmap which is just a
        # colored square (green:pass, red:fail, yellow:unimplemented). The second column holds
        # the disassembled instruction.

        # Add the first row: uncolored square in the first column, and "Initial State" in the second column.
        self.inst_list_ctrl.InsertItem(0, "")
        self.inst_list_ctrl.SetItem(0, 0, "Initial State")

        # Add the remaining rows: colored square in the first column, and the disassembled instruction in the second column.
        test_id = self.frame.wdb.GetTestId(test_name)
        cmd = 'SELECT Disasm,PC,ResultCode FROM Instructions WHERE TestId={} AND HartId={} ORDER BY PC ASC'.format(test_id, hart_id)
        cursor = self.frame.wdb.cursor
        cursor.execute(cmd)
        for idx, (disasm, pc, result_code) in enumerate(cursor.fetchall()):
            if result_code == 0:
                status = SquareStatus.PASS
            elif result_code >> 16 in (0x2, 0x3):
                status = SquareStatus.FAIL
            elif result_code >> 16 == 0x4:
                status = SquareStatus.UNIMPLEMENTED
            else:
                status = SquareStatus.UNSPECIFIED

            bg_color = SquareStatusColor.GetColor(status)
            self.inst_list_ctrl.InsertItem(idx+1, "")
            self.inst_list_ctrl.SetItemBackgroundColour(idx+1, bg_color)
            self.inst_list_ctrl.SetItem(idx+1, 0, disasm.replace('\t', ' '))
            self.inst_list_ctrl.SetItemData(idx+1, pc)

        self.__AlignInstListItems()
        self.inst_list_ctrl.Select(0)

    def __OnItemSelected(self, evt):
        idx = self.inst_list_ctrl.GetFirstSelected()
        if idx == 0:
            self.frame.test_debugger.test_viewer.ShowInitialState()
        else:
            pc = self.inst_list_ctrl.GetItemData(idx)
            self.frame.test_debugger.test_viewer.ShowInstruction(pc)

        self.__AlignInstListItems()

    def __AlignInstListItems(self):
        mnemonics = set()
        for idx in range(self.inst_list_ctrl.GetItemCount()):
            if idx == 0:
                continue
            mnemonic = self.inst_list_ctrl.GetItemText(idx).split(' ')[0]
            mnemonics.add(mnemonic)

        max_mnemonic_len = max(len(mnemonic) for mnemonic in mnemonics) + 4
        for idx in range(self.inst_list_ctrl.GetItemCount()):
            if idx == 0:
                continue
            disasm = self.inst_list_ctrl.GetItemText(idx)
            mnemonic = disasm.split(' ')[0]
            disasm = disasm.replace(mnemonic, '').lstrip()
            disasm = mnemonic.ljust(max_mnemonic_len) + disasm.replace('  ', ' ')
            self.inst_list_ctrl.SetItem(idx, 0, disasm)

        self.inst_list_ctrl.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.Layout()

class SpikeCodePanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Spike Code")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((400, -1))
        self.Layout()

    def OnLoadTest(self, test_name):
        # TODO cnyce
        pass

    def ShowInstruction(self, pc):
        self.help.SetLabel('GOTO {}'.format(pc))
        pass

class AtlasExperimentalCodePanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Atlas Experimental Code")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((450, -1))
        self.Layout()

    def OnLoadTest(self, test_name):
        # TODO cnyce
        pass

    def ShowInstruction(self, pc):
        self.help.SetLabel('GOTO {}'.format(pc))
        pass

class AtlasCppCodePanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Atlas C++ Code")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((450, -1))
        self.Layout()

    def OnLoadTest(self, test_name):
        # TODO cnyce
        pass

    def ShowInstruction(self, pc):
        self.help.SetLabel('GOTO {}'.format(pc))
        pass

class PythonTerminal(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Python Terminal")
        self.help.SetFont(mono12bold)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((wx.DisplaySize()[0], 120))
        self.Layout()

    def OnLoadTest(self, test_name):
        # TODO cnyce
        pass

class WorkloadsDB:
    def __init__(self, wdb_file):
        self.conn = sqlite3.connect(wdb_file)
        self.cursor = self.conn.cursor()

        self.cursor.execute('SELECT Id,TestName FROM RiscvTests')
        self.test_names_by_id = dict(self.cursor.fetchall())
        self.test_ids_by_name = dict((v,k) for k,v in self.test_names_by_id.items())

        self.cursor.execute('SELECT TestId,ResultCode FROM Instructions')
        self.failing_test_ids = set()
        for test_id, result_code in self.cursor.fetchall():
            # 2: pc invalid
            # 3: reg val invalid
            # 4: unimplemented inst
            if result_code >> 16 in (2,3,4):
                self.failing_test_ids.add(test_id)

        self.cursor.execute('SELECT RegName,RegType,RegIdx FROM Registers')
        self.reg_names_by_key = {}
        for reg_name, reg_type, reg_idx in self.cursor.fetchall():
            self.reg_names_by_key[(reg_type, reg_idx)] = reg_name

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

    def GetInitialDiffs(self, test_id, hart_id=0):
        self.cursor.execute('SELECT RegType,RegIdx,ExpectedInitVal,ActualInitVal FROM Registers WHERE TestId=? AND HartId=?', (test_id, hart_id))
        diffs = []
        for group_num, reg_idx, expected_val, actual_val in self.cursor.fetchall():
            diffs.append({'group_num': group_num, 'reg_idx': reg_idx, 'expected_val': expected_val, 'actual_val': actual_val})

        return diffs

    def GetInitialRegVals(self, test_id, hart_id=0):
        self.cursor.execute('SELECT RegType,RegIdx,ActualInitVal FROM Registers WHERE TestId=? AND HartId=?', (test_id, hart_id))
        reg_vals = []
        for reg_type, reg_idx, init_val in self.cursor.fetchall():
            reg_vals.append({'reg_type': reg_type, 'reg_idx': reg_idx, 'init_val': init_val})

        return reg_vals

    def GetRegisterName(self, group_num, reg_idx):
        return self.reg_names_by_key[(group_num, reg_idx)]

class SquareStatus(IntEnum):
    PASS = 0
    FAIL = 1
    UNIMPLEMENTED = 2
    UNSPECIFIED = 3

class SquareStatusColor:
    @staticmethod
    def GetColor(status):
        if status == SquareStatus.PASS:
            return wx.GREEN
        elif status == SquareStatus.FAIL:
            return wx.RED
        elif status == SquareStatus.UNIMPLEMENTED:
            return wx.YELLOW
        elif status == SquareStatus.UNSPECIFIED:
            return wx.WHITE

def CreateStatusSquare(parent, size, status):
    if isinstance(size, int):
        size = (size, size)
    square = wx.Panel(parent, -1, size=size)
    square.SetBackgroundColour(SquareStatusColor.GetColor(status))
    return square

if __name__ == "__main__":
    app = wx.App()

    # Enable frame inspection
    #import wx.lib.inspection
    #wx.lib.inspection.InspectionTool().Show()

    frame = AtlasIDE(sys.argv[1])
    frame.Show()
    app.MainLoop()
