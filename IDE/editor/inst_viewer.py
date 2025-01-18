import wx
from backend.sim_wrapper import SimWrapper

class InstViewer(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.SetBackgroundColour('green')
        self.frame = frame

    def LoadTest(self, test):
        import pdb; pdb.set_trace()

        with SimWrapper(self.frame.riscv_tests_dir, self.frame.sim_exe_path, test) as sim:
            init_regs = sim.GetRegisterValues()
            sim.BreakOnPostExecute()

            list_items = []
            while True:
                last_response = sim.Continue()
                assert isinstance(last_response, str)

                if last_response == 'post_execute':
                    inst = sim.GetCurrentInst()
                    if inst is None:
                        continue

                    mnemonic = inst.mnemonic
                    dasm = inst.dasm
                    list_items.append((mnemonic, dasm))
                elif last_response in ('no_response', 'sim_finished'):
                    break

            import pdb; pdb.set_trace()
