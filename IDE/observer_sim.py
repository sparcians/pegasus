from backend.sim_wrapper import SimWrapper
from backend.sim_api import *
from backend.dtypes import *
from backend.observers import *
import argparse

## Utility class which runs an observer on an Atlas simulation running in the background.
class ObserverSim:
    def __init__(self, riscv_tests_dir, sim_exe_path, test_name):
        self.riscv_tests_dir = riscv_tests_dir
        self.sim_exe_path = sim_exe_path
        self.test_name = test_name

    def RunObserver(self, obs):
        # You can ping the simulation as long as the SimWrapper is in scope.
        # See all the available APIs in IDE.backend.sim_api
        with SimWrapper(self.riscv_tests_dir, self.sim_exe_path, self.test_name) as sim:
            obs.OnPreSimulation(sim.endpoint)
            
            if obs.BreakOnPreExecute():
                atlas_break_action(sim.endpoint, 'pre_execute')

            if obs.BreakOnPreException():
                atlas_break_action(sim.endpoint, 'pre_exception')

            if obs.BreakOnPostExecute():
                atlas_break_action(sim.endpoint, 'post_execute')

            while True:
                break_method = atlas_continue(sim.endpoint)
                if not atlas_sim_alive(sim.endpoint):
                    break

                if break_method == 'pre_execute' and obs.BreakOnPreExecute():
                    obs.OnPreExecute(sim.endpoint)
                elif break_method == 'pre_exception' and obs.BreakOnPreException():
                    obs.OnPreException(sim.endpoint)
                elif break_method == 'post_execute' and obs.BreakOnPostExecute():
                    obs.OnPostExecute(sim.endpoint)

            obs.OnSimFinished(sim.endpoint)

        return obs.CreateReport()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Atlas IDE')
    parser.add_argument('--riscv-tests-dir', required=True, help='Path to the RISC-V tests directory')
    parser.add_argument('--sim-exe-path', required=True, help='Path to the RISC-V simulator executable')
    parser.add_argument('--test-name', required=True, help='Name of the test to run')
    args = parser.parse_args()

    obs = SanityCheckObserver('sanity_check.log')
    obs_sim = ObserverSim(args.riscv_tests_dir, args.sim_exe_path, args.test_name)
    report = obs_sim.RunObserver(obs)

    if report:
        report.Show()
