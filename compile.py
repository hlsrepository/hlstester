import os
import time
import re
import sys

# Put the directory where the current script is located as base_dir, assuming compile.py is placed in the instance folder
base_dir = os.path.dirname(os.path.abspath(__file__))

def create_vitis_tcl_script(kernel_path, testbench_path, extra_files, input_file_path, output_dir):
    """Generate a TCL script running by Vitis HLS"""
    tcl_script_path = os.path.join(base_dir, 'run_vitis.tcl')
    with open(tcl_script_path, 'w') as f:
        f.write('open_project -reset p1\n')
        f.write(f'add_files {kernel_path}\n')
        for file in extra_files:
            f.write(f'add_files {file}\n')
        f.write(f'add_files -tb {testbench_path}\n')
        f.write('open_solution -reset solution1 -flow_target vitis\n')
        f.write('set_top addAndDivideAndMultiply\n')
        f.write('set_part {xcvu9p-flga2104-2-i}\n')
        f.write('create_clock -period 10\n')
        f.write('set hls_exec 3\n')
        f.write(f'set kernel_path "{input_file_path}"\n')
        f.write(f'set output_dir "{output_dir}"\n')
        f.write(f'csim_design -argv "$kernel_path $output_dir"\n')
        f.write('if {$hls_exec == 1} {\n')
        f.write('    csynth_design\n')
        f.write('} elseif {$hls_exec == 2} {\n')
        f.write('    csynth_design\n')
        f.write('    cosim_design -argv "$kernel_path $output_dir"\n')
        f.write('} elseif {$hls_exec == 3} { \n')
        f.write('    csynth_design\n')
        f.write('    cosim_design -argv "$kernel_path $output_dir"\n')
        f.write('    export_design\n')
        f.write('} else {\n')
        f.write('    csynth_design\n')
        f.write('}\n')
        f.write('quit\n')
    return tcl_script_path

def run_tcl_script(tcl_script_path, vitis_bin):
    """Run the TCL script of Vitis HLS and time it"""
    command = f'{vitis_bin}/vitis-run --mode hls --tcl {tcl_script_path}'
    start_time = time.time()
    os.system(command)
    end_time = time.time()
    elapsed_time = end_time - start_time
    return elapsed_time

def check_report():
    """Check the comprehensive report to determine whether there are any errors"""
    log_file_path = os.path.join(base_dir, 'logs', 'hls_run_tcl.log')
    while not os.path.exists(log_file_path):
        time.sleep(1)
    with open(log_file_path, 'r') as file:
        report_content = file.read()
    error_patterns = ['Error', 'errors', 'Failed']
    result = 'PASS'
    for pattern in error_patterns:
        if re.search(pattern, report_content, re.IGNORECASE):
            result = 'FAIL'
            break
    metrics = {}
    return result, metrics

def main(kernel_path, testbench_path, input_file_path, output_dir, vitis_bin):
    """Main process: generate TCL, run HLS, check the results, and calculate the total time"""
    print("Starting Vitis HLS synthesis...")
    total_start_time = time.time()
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    tcl_script_path = create_vitis_tcl_script(kernel_path, testbench_path, [], input_file_path, output_dir)
    elapsed_time = run_tcl_script(tcl_script_path, vitis_bin)
    result, metrics = check_report()
    total_end_time = time.time()
    total_elapsed_time = total_end_time - total_start_time
    print(f"Vitis HLS synthesis completed. Total execution time: {total_elapsed_time:.2f} seconds")
    print(f"Synthesis time (HLS only): {elapsed_time:.2f} seconds")
    print(f"Result: {result}, Metrics: {metrics}")

if __name__ == "__main__":
    # kernel_path is fixed as kernel.cpp in the current instance folder
    kernel_path = os.path.join(base_dir, "kernel.cpp")
    # testbench_path is read from the command line. If it is not passed in, kernel_testbench0.cpp is used by default.
    if len(sys.argv) > 2:
        testbench_path = os.path.join(base_dir, sys.argv[2])
    else:
        testbench_path = os.path.join(base_dir, "kernel_testbench0.cpp")
    input_file_path = os.path.join(base_dir, "1.dat")
    output_dir = os.path.join(base_dir, "p")
    vitis_bin = '.../bin'
    
    main(kernel_path, testbench_path, input_file_path, output_dir, vitis_bin)
    sys.exit(1)
