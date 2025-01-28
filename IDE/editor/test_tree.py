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
                    if test.startswith(arch) and ext+"-" in test and not test.endswith(".dump"):
                        leaf = self.__GetOrAppendItem(ext_root, os.path.basename(test))
                        self.SetItemData(leaf, os.path.basename(test))

        orig_dir = os.getcwd()
        os.chdir(os.path.dirname(self.frame.sim_exe_path))

        test_py = os.path.join('..', '..', 'scripts', 'RunRiscVArchTest.py')
        test_py = os.path.abspath(test_py)
        assert os.path.isfile(test_py), 'Cannot find test script: ' + test_py

        os.chdir(orig_dir)

        self.Bind(wx.EVT_RIGHT_DOWN, self.__OnRightClick)

    def __GetOrAppendItem(self, parent, text):
        item, cookie = self.GetFirstChild(parent)
        while item:
            if self.GetItemText(item) == text:
                return item
            item, cookie = self.GetNextChild(parent, cookie)
        return self.AppendItem(parent, text)

    def __OnRightClick(self, event):
        item = self.GetSelection()
        if not item or not item.IsOk():
            return

        test = self.GetItemData(item)
        if not test:
            return

        menu = wx.Menu()
        menu.Append(1, "Load test")
        self.Bind(wx.EVT_MENU, self.__LoadTest, id=1)
        self.PopupMenu(menu)

    def __LoadTest(self, event):
        item = self.GetSelection()
        test = self.GetItemData(item)
        self.frame.workspace.LoadTest(test)
