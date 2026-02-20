#!/usr/bin/env python3

import os, sys
import re
import argparse
import subprocess
import multiprocessing
import functools

# Passing and total
PASSING_STATUS_RISCV_ARCH_RV32 = [284, 284]
PASSING_STATUS_RISCV_ARCH_RV64 = [364, 364]
PASSING_STATUS_TENSTORRENT_RV64 = [13155, 13155]

# Verbosity
be_noisy = False

def get_riscv_arch_tests(SUPPORTED_XLEN, directory):
    regex = re.compile(r'rv[36][24]')
    riscv_arch_tests = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if regex.match(file) and "dump" not in file:
                riscv_arch_tests.append(os.path.abspath(os.path.join(root, file)))

    riscv_arch_tests = [test for test in riscv_arch_tests if any(xlen in test for xlen in SUPPORTED_XLEN)]
    riscv_arch_tests.sort()

    tests = []
    for test in riscv_arch_tests:
        testname = test.split("/")[-1]
        tests.append([testname, test]);
    return tests


def get_tenstorrent_tests(SUPPORTED_EXTENSIONS, SUPPORTED_XLEN, directory):
    TENSTORRENT_SUPPORTED_EXTENSIONS = []
    # Tenstorrent tests are all RV64 even if name contains "rv32"
    if "rv64" in SUPPORTED_XLEN:
        TENSTORRENT_SUPPORTED_EXTENSIONS.extend(["rv64"+ext for ext in SUPPORTED_EXTENSIONS])
        TENSTORRENT_SUPPORTED_EXTENSIONS.extend(["rv32"+ext for ext in SUPPORTED_EXTENSIONS])
    else:
        return []

    regex = re.compile(r'rv[36][24]')
    tenstorrent_tests = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if regex.match(file) and "dump" not in file:
                tenstorrent_tests.append(os.path.abspath(os.path.join(root, file)))
    tenstorrent_tests = [test for test in tenstorrent_tests if any(ext+"_" in test for ext in TENSTORRENT_SUPPORTED_EXTENSIONS)]
    tenstorrent_tests.sort()

    tests = []
    bm_base_dir = "bare_metal"
    h_base_dir = "h_ext"

    for test in tenstorrent_tests:
        dirs = test.split('/')
        prefixes = []
        if (bm_base_dir in dirs):
            prefixes = dirs[dirs.index(bm_base_dir):-1]
        elif (h_base_dir in dirs):
            prefixes = dirs[dirs.index(h_base_dir):-1]
        else:
            print("WARNING: Bad test path", test)
        prefix = "_".join(prefixes) + "_"
        testname = prefix + os.path.basename(test)
        testname = testname.replace("rv32", "rv64")
        tests.append([testname, test]);
    return tests


# Function to run a single test and append to the appropriate queue
def run_test(testname, wkld, output_dir, passing_tests, failing_tests, timeout_tests, executable):
    if be_noisy:
        print("Running", testname)
    rv32_test = "rv32" in testname
    logname = output_dir + testname + ".log"
    instlogname = output_dir + testname + ".instlog"
    error_dump = output_dir + testname + ".error"
    isa_string = "gcbvh_zicsr_zifencei_zicond_zfh_zbkb_zbkx_zicboz"
    isa_string = "rv32"+isa_string if rv32_test else "rv64"+isa_string
    pegasus_cmd = [executable,
                 "--debug-dump-filename", error_dump,
                 "-p", "top.core0.params.isa", isa_string, "-w", wkld]

    test_passed = False
    try:
        with open(logname, "w") as f:
            result = subprocess.run(pegasus_cmd, stdout=f, stderr=f, timeout=300)
            if result.returncode == 0:
                test_passed = True

    except subprocess.TimeoutExpired:
        timeout_tests.append(testname)

    if test_passed:
        # Remove log files if test passed
        os.remove(logname)
        passing_tests.append(testname)
    else:
        error = 'UNKNOWN'
        with open(logname, "r") as log:
            for line in log:
                if line.startswith('MAGIC') or line.startswith('FAIL'):
                    error = line.strip()
                    break

        failing_tests.append([testname, error])


def run_tests_in_parallel(tests, passing_tests, failing_tests, timeout_tests, output_dir, executable):
    import concurrent.futures

    # Limit number of concurrent tests running to the number of CPUs on this machine
    executor = concurrent.futures.ThreadPoolExecutor(max_workers=multiprocessing.cpu_count())
    jobs = []
    for testname, wkld in tests:
        # Submit job
        jobs.append(executor.submit(run_test, testname, wkld, output_dir, passing_tests, failing_tests, timeout_tests, executable))

    # Wait for jobs to complete with a timeout of 5 minutes
    for j in concurrent.futures.as_completed(jobs):
        try:
            result = j.result(timeout=300)
        except TimeoutError:
            print("TIMEOUT:", j)
            continue


def run_tests_serially(tests, passing_tests, failing_tests, timeout_tests, output_dir, executable):
    for testname, wkld in tests:
        print ("Running test:", testname)
        run_test(testname, wkld, output_dir, passing_tests, failing_tests, timeout_tests, executable)


def extract_sparta_failures(log_file, failure_dict):
    with open(log_file, 'r') as fin:
        for line in fin.readlines():
            if line.find('FAILED on line') != -1:
                parts = line.split(' FAILED on line ')
                assert len(parts) == 2

                # Parse a C++ test failure line and produce a concise summary.
                #
                # Example input for regex:
                #     Test 'sim_state_truth->current_uid' FAILED on line 115 in file /path/to/FlushWorkload_test.cpp. Value: '1057' should equal '1058'
                #
                pattern = re.compile(r"Test\s+'([^']+)'\s+FAILED\s+on\s+line\s+(\d+)\s+in\s+file\s+([^\s]+)")
                match = pattern.search(line)
                if not match:
                    continue

                test_expr, _, _ = match.groups()
                similar_failures = failure_dict.get(test_expr, set())
                similar_failures.add(os.path.basename(log_file))
                failure_dict[test_expr] = similar_failures


def print_sparta_failures():
    failure_dict = {}
    for entry in os.listdir(os.getcwd()):
        if entry.endswith('.log'):
            log_path = os.path.join(os.getcwd(), entry)
            log_path = os.path.abspath(log_path)
            extract_sparta_failures(log_path, failure_dict)

    for test_expr in sorted(failure_dict.keys()):
        log_files = failure_dict[test_expr]
        print(f"Test Expression: '{test_expr}' failed in {len(log_files)} log file(s):")
        for log_file in sorted(log_files):
            print(f"    - {log_file}")

    if not failure_dict:
        return

    print()

    failures_per_log_file = {}
    for test_expr, log_files in failure_dict.items():
        for log_file in log_files:
            failures = failures_per_log_file.get(log_file, set())
            failures.add(test_expr)
            failures_per_log_file[log_file] = failures

    log_file_with_most_failures = max(failures_per_log_file.items(), key=lambda item: len(item[1]))
    log_file, failures = log_file_with_most_failures
    print(f"Log file with the most failures: {log_file} ({len(failures)} failures)")
    for test_expr in sorted(failures):
        print(f"    - {test_expr}")

    print()

    log_file_with_least_failures = min(failures_per_log_file.items(), key=lambda item: len(item[1]))
    log_file, failures = log_file_with_least_failures
    print(f"Log file with the least failures: {log_file} ({len(failures)} failures)")
    for test_expr in sorted(failures):
        print(f"    - {test_expr}")

    print()

def main():
    parser = argparse.ArgumentParser(description="Script to run the Tenstorrent architecture tests on Pegasus")
    parser.add_argument("--extensions", type=str, nargs="+", help="The extensions to test (e.g. i, m, a, f, d)")
    parser.add_argument("--output", type=str, help="Where logs and error files should go")
    xlen_group = parser.add_mutually_exclusive_group()
    xlen_group.add_argument("--rv32-only", action='store_true', help="Only run RV32 tests (skip RV64)")
    xlen_group.add_argument("--rv64-only", action='store_true', help="Only run RV64 tests (skip RV32)")
    parser.add_argument("-v","--verbose", action='store_true', default=False, help="Be noisy")
    parser.add_argument("--riscv-arch", type=str, help="The directory of the built RISC-V Arch tests")
    parser.add_argument("--tenstorrent", type=str, help="The directory of the built Tenstorrent tests")
    parser.add_argument("--pegasus-exe", type=str, default="./pegasus", help="Path to the Pegasus executable (default: ./pegasus)")
    parser.add_argument("--serial", action='store_true', default=False, help="Run tests serially instead of in parallel")
    parser.add_argument("--expected-pass-rate", type=float, help="Expected pass rate (for CI purposes only)")
    args = parser.parse_args()

    # Extract the RISC-V tarball if needed
    if args.riscv_arch is not None:
        if not os.path.isdir(args.riscv_arch):
            tarball = args.riscv_arch + ".tar.gz"
            if os.path.isfile(tarball):
                print("Extracting RISC-V arch tests from", tarball)
                subprocess.run(["tar", "-xzf", tarball], check=True)
                args.riscv_arch = os.getcwd() + "/" + os.path.basename(args.riscv_arch)

    global be_noisy

    ###########################################################################
    # Process arguments
    SUPPORTED_XLEN = ["rv32", "rv64"]
    if args.rv32_only:
        print("Skipping RV64 tests")
        SUPPORTED_XLEN = ["rv32"]
    elif args.rv64_only:
        print("Skipping RV32 tests")
        SUPPORTED_XLEN = ["rv64"]

    output_dir = "./"
    if args.output:
        output_dir = args.output
        try:
            os.makedirs(output_dir, exist_ok=True)
            output_dir += '/'
        except Exception as e:
            print("ERROR: Could not make output directory", output_dir, e)
            sys.exit(1)

    if args.rv32_only and args.rv64_only:
        print("ERROR: Cannot set both \'--rv32-only\' and \'--rv64-only\'")
        sys.exit(1)

    be_noisy = args.verbose

    ###########################################################################
    # Make sure we're in the correct sim directory for pegasus
    if not os.path.isfile(args.pegasus_exe):
        print("ERROR: Pegasus executable not found:", args.pegasus_exe)
        sys.exit(1)

    ###########################################################################
    # Get tests
    tests = []
    if args.riscv_arch:
        tests.extend(get_riscv_arch_tests(SUPPORTED_XLEN, args.riscv_arch))
    if args.tenstorrent:
        tests.extend(get_tenstorrent_tests(SUPPORTED_XLEN, args.tenstorrent))

    if args.extensions:
        tests = [test for test in tests if any(ext in test[0] for ext in args.extensions)]

    skip_tests = [
        "rv64mi-p-breakpoint",       # Not supported yet
        "rv32mi-p-breakpoint",
        "rv64mi-p-instret_overflow", # Not supported yet
        "rv32mi-p-instret_overflow",
        "rv64ssvnapot-p-napot",      # Not supported yet
        "rv64mzicbo-p-zero"          # Need to debug
    ]
    for skip_test in skip_tests:
        if be_noisy:
            print("Skipping", skip_test)
        tests = [test for test in tests if skip_test not in test]

    if len(tests) == 0:
        print("Failed to find any tests to run! Need to specify --riscv-arch <dir> or --tenstorrent <dir>")
        sys.exit(1)

    ###########################################################################
    # Run!
    print("Running " + str(len(tests)) + " arch tests...")
    passing_tests = []
    failing_tests = []
    timeout_tests = []

    if args.serial:
        run_tests_serially(tests, passing_tests, failing_tests, timeout_tests, output_dir, args.pegasus_exe)
    else:
        run_tests_in_parallel(tests, passing_tests, failing_tests, timeout_tests, output_dir, args.pegasus_exe)

    ###########################################################################
    # Report results
    num_passed = 0
    if passing_tests:
        print("PASSED:")
        for test in passing_tests:
            print("\t" + test)
            num_passed += 1

    if failing_tests:
        print("FAILED:")
        for test,error in failing_tests:
            print("\t" + test + ": " + error)

    if timeout_tests:
        print("TIMED OUT:")
        for test in timeout_tests:
            print("\t" + test)
    print()

    print_sparta_failures()

    pass_rate = num_passed/len(tests)
    print("PASS RATE: {:.2%} ({}/{})".format(pass_rate, num_passed, len(tests)))

    expected_passing_tests = 0
    total_tests = 0
    if args.riscv_arch:
        if not args.rv64_only:
            expected_passing_tests += PASSING_STATUS_RISCV_ARCH_RV32[0]
            total_tests += PASSING_STATUS_RISCV_ARCH_RV32[1]
        if not args.rv32_only:
            expected_passing_tests += PASSING_STATUS_RISCV_ARCH_RV64[0]
            total_tests += PASSING_STATUS_RISCV_ARCH_RV64[1]
    if args.tenstorrent:
        expected_passing_tests += PASSING_STATUS_TENSTORRENT_RV64[0]
        total_tests += PASSING_STATUS_TENSTORRENT_RV64[1]

    expected_pass_rate = expected_passing_tests/total_tests
    print("EXPECTED RATE: {:.2%} ({}/{})".format(expected_pass_rate, expected_passing_tests, total_tests))

    if args.expected_pass_rate is not None:
        if round(pass_rate*100, 2) < args.expected_pass_rate:
            print(f"ERROR: Pass rate {pass_rate:.2%} is below expected pass rate of {args.expected_pass_rate}!")
            sys.exit(1)
    elif (num_passed < expected_passing_tests):
        print("ERROR: failed!")

if __name__ == "__main__":
    main()
