import wx, os, json, re
import wx.grid
import wx.py.shell
import wx.lib.scrolledpanel
from backend.c_dtypes import uint64_t, int64_t

class TestDebugger(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.frame = frame

        hsplitter = wx.SplitterWindow(self, style=wx.SP_LIVE_UPDATE)
        self.state_viewer = StateViewer(hsplitter, frame)

        locals = {'uint64_t': uint64_t, 'int64_t': int64_t}
        self.shell = PyShell(hsplitter, locals=locals)

        hsplitter.SplitHorizontally(self.state_viewer, self.shell)
        hsplitter.SetMinimumPaneSize(50)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(hsplitter, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

        w,h = self.GetSize()
        hsplitter.SetSashPosition(h-200)

    def SetStateQuery(self, state_query):
        self.state_viewer.SetStateQuery(state_query)

    def LoadInst(self, pc, inst):
        self.state_viewer.LoadInst(pc, inst)

    def LoadInitialState(self):
        self.state_viewer.LoadInitialState()

class PyShell(wx.py.shell.Shell):
    def __init__(self, *args, **kwargs):
        wx.py.shell.Shell.__init__(self, *args, **kwargs)
        self.Bind(wx.EVT_KEY_DOWN, self.__OnKeyPress)
        self.command_history = []
        self.command_history_cursor = None

    def __OnKeyPress(self, event):
        if event.GetKeyCode() == wx.WXK_RETURN:
            command = self.getCommand()
            if self.command_history and command == self.command_history[-1]:
                self.command_history.pop()
            self.command_history.append(command)
            self.command_history_cursor = len(self.command_history)
            event.Skip()
        elif event.GetKeyCode() == wx.WXK_UP and self.command_history:
            if self.command_history_cursor > 0:
                self.command_history_cursor -= 1
                self.clearCommand()
                self.write(self.command_history[self.command_history_cursor])
                self.SetInsertionPointEnd()
        elif event.GetKeyCode() == wx.WXK_DOWN and self.command_history:
            if self.command_history_cursor < len(self.command_history) - 1:
                self.command_history_cursor += 1
                self.clearCommand()
                self.write(self.command_history[self.command_history_cursor])
                self.SetInsertionPointEnd()
        else:
            event.Skip()

class StateViewer(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.frame = frame
        self.state_query = None
        self.SetBackgroundColour('white')

        meta_panel = wx.Panel(self)
        self.pc_label = wx.StaticText(meta_panel, label="pc:0x00000000")
        self.priv_label = wx.StaticText(meta_panel, label="priv:")
        self.changes_label = wx.StaticText(meta_panel, label="changes:")

        self.pc_label.Bind(wx.EVT_RIGHT_DOWN, self.__OnRightClick)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.pc_label.SetFont(mono10)
        self.priv_label.SetFont(mono10)
        self.changes_label.SetFont(mono10)

        row1_sizer = wx.BoxSizer(wx.HORIZONTAL)
        row1_sizer.Add(self.pc_label)
        row1_sizer.AddSpacer(25)
        row1_sizer.Add(self.priv_label)
        row1_sizer.AddSpacer(25)
        row1_sizer.Add(self.changes_label)
        meta_panel.SetSizer(row1_sizer)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(meta_panel, 0, wx.EXPAND)
        sizer.Add(wx.StaticLine(self), 0, wx.EXPAND)

        atlas_root = os.path.join(os.path.dirname(__file__), '..', '..')

        int_json = os.path.join(atlas_root, 'arch', 'rv64', 'reg_int.json')
        with open(int_json, 'r') as fin:
            int_names = [reg['name'] for reg in json.load(fin)]
            int_names = [name for name in int_names if name.startswith('x')]

        fp_json = os.path.join(atlas_root, 'arch', 'rv64', 'reg_fp.json')
        with open(fp_json, 'r') as fin:
            fp_names = [reg['name'] for reg in json.load(fin)]

        csr_json = os.path.join(atlas_root, 'arch', 'rv64', 'reg_csr.json')
        with open(csr_json, 'r') as fin:
            csr_names = [reg['name'] for reg in json.load(fin)]

        self.regval_grids = {
            'x':   RegisterGrid(self, int_names, color_nonzero=True),
            'f':   RegisterGrid(self, fp_names, color_nonzero=True),
            'csr': RegisterGrid(self, csr_names)
        }

        cpp_notebook = wx.Notebook(self)

        def AddTab(notebook, tab_name, handler_subdir=None, handler_file=None):
            if handler_subdir is None and handler_file is None:
                handler_subdir = tab_name

            if handler_subdir is not None:
                atlas_root = os.path.join(os.path.dirname(__file__), '..', '..')
                handler_dir = os.path.join(atlas_root, 'core', 'inst_handlers', handler_subdir)

                cpp_code = None
                for handler_file in os.listdir(handler_dir):
                    if handler_file.endswith('.cpp'):
                        with open(os.path.join(handler_dir, handler_file), 'r') as fin:
                            cpp_code = fin.read()
                            break
            else:
                with open(handler_file, 'r') as fin:
                    cpp_code = fin.read()

            cpp_scrolled_panel = wx.lib.scrolledpanel.ScrolledPanel(notebook)
            cpp_scrolled_panel.SetupScrolling()

            mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
            cpp_code_panel = wx.TextCtrl(cpp_scrolled_panel, style=wx.TE_MULTILINE|wx.TE_READONLY)
            cpp_code_panel.SetFont(mono10)
            cpp_code_panel.SetValue(cpp_code)

            search_bar = wx.SearchCtrl(cpp_scrolled_panel)
            search_bar.Bind(wx.EVT_SEARCH, self.__OnCppSearch)
            search_bar.Bind(wx.EVT_SEARCH_CANCEL, self.__OnCppSearchCancelled)

            cpp_scrolled_panel_sizer = wx.BoxSizer(wx.VERTICAL)
            cpp_scrolled_panel_sizer.Add(cpp_code_panel, 1, wx.EXPAND)
            cpp_scrolled_panel_sizer.Add(search_bar, 0, wx.EXPAND)
            cpp_scrolled_panel.SetSizer(cpp_scrolled_panel_sizer)

            notebook.AddPage(cpp_scrolled_panel, tab_name)

        AddTab(cpp_notebook, 'a')
        AddTab(cpp_notebook, 'd')
        AddTab(cpp_notebook, 'f')
        AddTab(cpp_notebook, 'i')
        AddTab(cpp_notebook, 'm')
        AddTab(cpp_notebook, 'zicsr')
        AddTab(cpp_notebook, 'zifencei')

        atlas_root = os.path.join(os.path.dirname(__file__), '..', '..')
        execute_cpp_file = os.path.join(atlas_root, 'core', 'Exception.cpp')
        AddTab(cpp_notebook, 'trap', handler_file=execute_cpp_file)

        hsizer = wx.BoxSizer(wx.HORIZONTAL)
        hsizer.Add(self.regval_grids['x'])
        hsizer.Add(self.regval_grids['f'])
        hsizer.Add(self.regval_grids['csr'])
        hsizer.Add(cpp_notebook, 1, wx.LEFT|wx.EXPAND, 10)
        sizer.Add(hsizer, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def SetStateQuery(self, state_query):
        self.state_query = state_query
        self.LoadInitialState()

    def LoadInst(self, pc, inst):
        snapshot = self.state_query.GetSnapshot(pc)
        changes = snapshot.getChanges()

        self.pc_label.SetLabel('pc:'+pc)

        changes_list = []
        for reg_name, (prev_val, curr_val) in changes.items():
            prev_val = hex(int(prev_val))
            curr_val = hex(int(curr_val))
            changes_list.append('{}({})'.format(reg_name, '{}->{}'.format(prev_val, curr_val)))

        if not changes_list:
            changes_list.append('none')

        self.changes_label.SetLabel('changes: ' + ', '.join(changes_list))
        for _, grid in self.regval_grids.items():
            grid.SetRegValues(snapshot)

    def LoadInitialState(self):
        if not self.state_query:
            return

        snapshot = self.state_query.GetInitialState()
        pc = snapshot.getPC()
        priv = ['U', 'S', 'H', 'M'][int(snapshot.getRegValue('resv_priv'))]
        self.pc_label.SetLabel('pc:0x{:08x}'.format(pc))
        self.priv_label.SetLabel('priv:'+priv)
        self.changes_label.SetLabel('changes: ----')

        for _, grid in self.regval_grids.items():
            grid.SetRegValues(snapshot)

    def __OnCppSearch(self, event):
        search_bar = event.GetEventObject()
        search_text = search_bar.GetValue()
        cpp_code_panel = search_bar.GetParent().GetChildren()[0]
        cpp_code = cpp_code_panel.GetValue()

        curr_sel = cpp_code_panel.GetSelection()
        if curr_sel[0] == curr_sel[1]:
            start = cpp_code.find(search_text)
        else:
            start = cpp_code.find(search_text, curr_sel[1])
            if start == -1:
                start = cpp_code.find(search_text)

        if start == -1:
            return

        end = start + len(search_text)
        cpp_code_panel.SetSelection(start, end)
        cpp_code_panel.ShowPosition(start)

    def __OnCppSearchCancelled(self, event):
        search_bar = event.GetEventObject()
        cpp_code_panel = search_bar.GetParent().GetChildren()[0]
        cpp_code_panel.SetSelection(0, 0)

    def __OnRightClick(self, event):
        text = self.pc_label.GetLabel()
        if not text:
            return
        
        menu = wx.Menu()
        menu.Append(1, 'Copy')
        self.Bind(wx.EVT_MENU, self.__OnCopy, id=1)
        self.PopupMenu(menu)
        menu.Destroy()

    def __OnCopy(self, event):
        text = self.pc_label.GetLabel()
        val = text.split(':')[-1]
        val = 'uint64_t({})'.format(text.split(':')[-1])
        wx.TheClipboard.Open()
        wx.TheClipboard.SetData(wx.TextDataObject(val))
        wx.TheClipboard.Close()

class RegisterGrid(wx.grid.Grid):
    def __init__(self, parent, reg_names, color_nonzero=False):
        wx.grid.Grid.__init__(self, parent)
        self.reg_names = reg_names
        self.color_nonzero = color_nonzero

        self.CreateGrid(0, 2)
        self.HideRowLabels()
        self.HideColLabels()
        self.EnableEditing(False)
        self.DisableDragGridSize()

        mono9 = wx.Font(9, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.SetDefaultCellFont(mono9)

        self.Bind(wx.grid.EVT_GRID_CELL_RIGHT_CLICK, self.__OnCellRightClick)

    def SetRegValues(self, snapshot):
        self.ClearGrid()
        for row in range(self.GetNumberRows()):
            self.SetCellBackgroundColour(row, 0, 'white')
            self.SetCellBackgroundColour(row, 1, 'white')

        # Note that screen space becomes a problem when we try to display
        # every CSR value. We should only display the ones that are non-zero.
        # The regex below is to distinguish between integer/fp registers and CSRs.
        xpattern = r"^x([0-9]|[12][0-9]|3[01])$"
        fpattern = r"^f([0-9]|[12][0-9]|3[01])$"
        nvpairs = []

        for reg_name in self.reg_names:
            reg_val = snapshot.getRegValue(reg_name)
            if reg_val > 0 or re.match(xpattern, reg_name) or re.match(fpattern, reg_name):
                nvpairs.append((reg_name, reg_val))

        num_rows_have = self.GetNumberRows()
        num_rows_need = len(nvpairs)
        if num_rows_need > num_rows_have:
            self.AppendRows(num_rows_need - num_rows_have)

        for row, (reg_name, reg_val) in enumerate(nvpairs):
            reg_val = hex(int(reg_val)).lstrip('0x')
            reg_val = reg_val.zfill(16)
            reg_val = '0x' + reg_val
            nvpairs[row] = (reg_name, reg_val)

            self.SetCellValue(row, 0, reg_name)
            self.SetCellValue(row, 1, reg_val)

            changes = snapshot.getChanges()
            if changes is not None and reg_name in snapshot.getChanges():
                self.SetCellBackgroundColour(row, 0, 'yellow')
                self.SetCellBackgroundColour(row, 1, 'yellow')
            elif int(reg_val, 16) and self.color_nonzero:
                self.SetCellBackgroundColour(row, 0, 'cyan')
                self.SetCellBackgroundColour(row, 1, 'cyan')

            self.ShowRow(row)

        for row in range(len(nvpairs), self.GetNumberRows()):
            self.HideRow(row)

        max_reg_val_len = max(len(reg_val) for _, reg_val in nvpairs) - 2
        for row in range(len(nvpairs)):
            reg_val_label = self.GetCellValue(row, 1).lstrip('0x')
            reg_val_label = reg_val_label.zfill(max_reg_val_len)
            reg_val_label = '0x' + reg_val_label
            self.SetCellValue(row, 1, reg_val_label)

        self.AutoSize()

    def __OnCellRightClick(self, event):
        row = event.GetRow()
        col = event.GetCol()
        if col == 1:
            def copy_text(event):
                reg_val = self.GetCellValue(row, col)
                copy_text = 'uint64_t({})'.format(reg_val)

                wx.TheClipboard.Open()
                wx.TheClipboard.SetData(wx.TextDataObject(copy_text))
                wx.TheClipboard.Close()

            menu = wx.Menu()
            menu.Append(1, 'Copy')
            self.Bind(wx.EVT_MENU, copy_text, id=1)
            self.PopupMenu(menu)
