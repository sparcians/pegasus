import wx, os
from backend.sim_endpoint import SimEndpoint
from backend.sim_endpoint import Interactivity

class InstViewer(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.SetBackgroundColour('green')
        self.frame = frame

    def LoadTest(self, test):
        test_path = os.path.join(self.frame.riscv_tests_dir, test)

        sim_endpoint = SimEndpoint()
        sim_endpoint.Bind(Interactivity.PostInit, self.__OnPostInit)
        sim_endpoint.Bind(Interactivity.PreExecute, self.__OnPreExecute)
        sim_endpoint.Bind(Interactivity.PreException, self.__OnPreException)
        sim_endpoint.Bind(Interactivity.PostExecute, self.__OnPostExecute)
        sim_endpoint.Bind(Interactivity.SimFinished, self.__OnSimFinished)
        sim_endpoint.RunSimulation(self.frame.sim_exe_path, test_path)

    def __OnPostInit(self, obj):
        print ('PostInit')

    def __OnPreExecute(self, obj):
        print ('PreExecute')

    def __OnPreException(self, obj):
        print ('PreException')

    def __OnPostExecute(self, obj):
        print ('PostExecute')

    def __OnSimFinished(self, obj):
        print ('SimFinished')
