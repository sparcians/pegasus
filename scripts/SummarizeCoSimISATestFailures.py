#! /usr/bin/env python3

import argparse, os, re

parser = argparse.ArgumentParser(description="Summarize Co-Simulation ISA Test Failures")
parser.add_argument("--test-dir", type=str, required=True, help="Directory containing the test log files (*.log)")
parser.add_argument("--brief", action='store_true', default=False, help="Produce a brief summary of failures")
args = parser.parse_args()

def ExtractFailure(log_file, failure_dict):
    with open(log_file, 'r') as fin:
        for line in fin.readlines():
            if line.find('FAILED on line') != -1:
                # Line is something like:
                #   Test 'sim_state_truth->current_uid' FAILED on line 115 in file /home/cnyce/pegasus/test/cosim/cosim_workload/flush_workload/FlushWorkload_test.cpp. Value: '1057' should equal '1058'
                # Turn this into a summary like:
                #   FAILED on line 115 in FlushWorkload_test.cpp: sim_state_truth->current_uid
                parts = line.split(' FAILED on line ')
                assert len(parts) == 2

                # Parse a C++ test failure line and produce a concise summary.
                #
                # Example input:
                #     Test 'sim_state_truth->current_uid' FAILED on line 115 in file /path/to/FlushWorkload_test.cpp. Value: '1057' should equal '1058'
                #
                # Output:
                #     FAILED on line 115 in FlushWorkload_test.cpp: sim_state_truth->current_uid
                #
                #
                pattern = re.compile(r"Test\s+'([^']+)'\s+FAILED\s+on\s+line\s+(\d+)\s+in\s+file\s+([^\s]+)")
                match = pattern.search(line)
                if not match:
                    return

                test_expr, line_num, filepath = match.groups()
                filename = os.path.basename(filepath)

                summary = f"FAILED on line {line_num} in {filename}: {test_expr}"
                similar_failures = failure_dict.get(test_expr, [])
                similar_failures.append(os.path.basename(log_file))
                failure_dict[test_expr] = similar_failures
                return

# Walk the current directory and find all .log files
failure_dict = {}
for entry in os.listdir(args.test_dir):
    if entry.endswith('.log'):
        log_path = os.path.join(args.test_dir, entry)
        log_path = os.path.abspath(log_path)
        ExtractFailure(log_path, failure_dict)

# Full failure reporting
def PrintFullFailures(failure_dict):
    for test_expr in sorted(failure_dict.keys()):
        log_files = failure_dict[test_expr]
        print(f"Test Expression: '{test_expr}' failed in {len(log_files)} log file(s):")
        for log_file in log_files:
            print(f"    - {log_file}")

# Brief failure reporting
def PrintBriefFailures(failure_dict):
    for test_expr in sorted(failure_dict.keys()):
        log_files = failure_dict[test_expr]
        print(f"Test Expression: '{test_expr}' failed in {len(log_files)} log file(s). First few:")
        for log_file in log_files[:5]:
            print(f"    - {log_file}")
        if len(log_files) > 5:
            print(f"    ... and {len(log_files) - 5} more.")

# Write the summary to stdout, sorted by test expression
if failure_dict:
    print("\nCo-Simulation ISA Test Failures Summary:")
    if not args.brief:
        PrintFullFailures(failure_dict)
    else:
        PrintBriefFailures(failure_dict)
else:
    print("\nNo Co-Simulation ISA Test Failures Found.")
