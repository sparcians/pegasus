#!/usr/bin/env python3

import argparse, os
import subprocess
import multiprocessing

parser = argparse.ArgumentParser(description='Run Co-Simulation Architecture Tests')
parser.add_argument('--sim-exe-path', type=str, required=True, help='Path to the co-simulation executable')
parser.add_argument('--isa-test-dir', type=str, required=True, help='Path to the ISA test directory')
args = parser.parse_args()

if not os.path.exists(args.sim_exe_path):
    print(f"Error: Simulation executable not found at {args.sim_exe_path}")
    exit(1)

if not os.path.exists(args.isa_test_dir):
    tarball = args.isa_test_dir + ".tar.gz"
    if os.path.exists(tarball):
        print(f"Extracting ISA test directory from {tarball}...")
        os.system(f"tar -xzf {tarball}")
    else:
        print(f"Error: Could not find ISA test directory or tarball at {args.isa_test_dir}[.tar.gz]")
        exit(1)
else:
    # Get the directory name from the provided path
    target = os.path.abspath(args.isa_test_dir)
    dir_name = os.path.basename(os.path.normpath(args.isa_test_dir))
    link_name = os.path.join(os.getcwd(), dir_name)
    os.symlink(target, link_name, target_is_directory=True)

