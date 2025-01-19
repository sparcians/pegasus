import os, subprocess, wx

class TestTreeCtrl(wx.TreeCtrl):
    def __init__(self, parent, frame):
        wx.TreeCtrl.__init__(self, parent, style=wx.TR_HAS_BUTTONS | wx.TR_HIDE_ROOT)

        self.frame = frame
        self.root = self.AddRoot("Tests")

        SUPPORTED_ARCHS = ["rv32", "rv64"]
        SUPPORTED_EXTENSIONS = ["mi", "si", "ui", "um", "ua", "uf", "ud"]
        riscv_tests_dir = self.frame.riscv_tests_dir

        tests = os.listdir(riscv_tests_dir)
        tests.sort()

        for arch in SUPPORTED_ARCHS:
            arch_root = self.__GetOrAppendItem(self.root, arch)
            for ext in SUPPORTED_EXTENSIONS:
                ext_root = self.__GetOrAppendItem(arch_root, ext)
                for test in tests:
                    if test.startswith(arch) and ext+"-" in test:
                        leaf = self.__GetOrAppendItem(ext_root, os.path.basename(test))
                        self.SetItemData(leaf, os.path.basename(test))

        orig_dir = os.getcwd()
        os.chdir(os.path.dirname(self.frame.sim_exe_path))

        test_py = os.path.join('..', '..', 'scripts', 'RunRiscVArchTest.py')
        test_py = os.path.abspath(test_py)
        assert os.path.isfile(test_py), 'Cannot find test script: ' + test_py

        #self.__CaptureBaseline(test_py, 'rv32')
        #self.__CaptureBaseline(test_py, 'rv64')

        os.chdir(orig_dir)

        self.Bind(wx.EVT_RIGHT_DOWN, self.__OnRightClick)

    def __GetOrAppendItem(self, parent, text):
        item, cookie = self.GetFirstChild(parent)
        while item:
            if self.GetItemText(item) == text:
                return item
            item, cookie = self.GetNextChild(parent, cookie)
        return self.AppendItem(parent, text)

    def __CaptureBaseline(self, test_py, arch):
        arch_node = self.GetFirstChild(self.root)[0]
        while arch_node:
            if self.GetItemText(arch_node) == arch:
                break
            arch_node = self.GetNextSibling(arch_node)

        if not arch_node or not arch_node.IsOk():
            return

        test_cmd = '{} {} {}'.format(test_py, arch, self.frame.riscv_tests_dir)

        print ('Running all RISC-V tests for {} to establish a baseline...'.format(arch))
        proc = subprocess.Popen(test_cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = proc.communicate()
        if proc.returncode != 0:
            print ('Failed to capture baseline tests for', arch)
            self.Delete(arch_node)
            return

        passing = []
        capture_passing = False

        failing = []
        capture_failing = False

        for line in stdout.splitlines():
            line = line.decode('utf-8')
            if line.strip() == 'PASSED:':
                capture_passing = True
                capture_failing = False
                continue

            if line.strip() == 'FAILED:':
                capture_passing = False
                capture_failing = True
                continue

            if capture_passing and line:
                passing.append(line.strip())
                continue

            if capture_failing and line:
                failing.append(line.strip())
                continue

        # Recursively walk every node in the tree, and compare its user data to the passing/failing lists.
        # The goal is to give all failing tests red text.
        def RecurseSetItemColor(item, failing):
            if self.GetItemData(item) in failing:
                self.SetItemTextColour(item, 'red')

            child, cookie = self.GetFirstChild(item)
            while child:
                RecurseSetItemColor(child, failing)
                child, cookie = self.GetNextChild(item, cookie)

        RecurseSetItemColor(arch_node, failing)

    def __OnRightClick(self, event):
        item = self.GetSelection()
        if not item or not item.IsOk():
            return

        test = self.GetItemData(item)
        if not test:
            return

        page_idx = self.frame.notebook.GetSelection()
        workspace_name = self.frame.notebook.GetPageText(page_idx)

        menu = wx.Menu()
        menu.Append(1, "Load test in active workspace ({})".format(workspace_name))

        self.Bind(wx.EVT_MENU, self.__LoadTestInActiveWorkspace, id=1)
        self.PopupMenu(menu)

    def __LoadTestInActiveWorkspace(self, event):
        item = self.GetSelection()
        test = self.GetItemData(item)
        selected_tab = self.frame.notebook.GetSelection()
        workspace = self.frame.workspaces[selected_tab]
        workspace.LoadTest(test)
