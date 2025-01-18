import wx

class InstEditor(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.SetBackgroundColour('blue')
        self.frame = frame

    def LoadInst(self, pc, inst):
        print ('LoadInst at PC {} for mnemonic {}'.format(pc, inst.mnemonic))
