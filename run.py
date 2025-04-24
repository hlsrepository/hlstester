import os
import shutil
import subprocess
import random

def create_folders_and_copy_files(source_dir, parent_dir, num_folders, all_files):
    """
    Create E1_1 to E1_{num_folders} under parent_dir, copying files from source_dir.
    No file content modification is performed.
    """
    for i in range(1, num_folders + 1):
        instance_dir = os.path.join(parent_dir, f"E1_{i}")
        os.makedirs(instance_dir, exist_ok=True)

        for filename in all_files:
            src_file = os.path.join(source_dir, filename)
            dest_file = os.path.join(instance_dir, filename)

            #  Copy the file if it exists in source_dir
            if os.path.isfile(src_file):
                shutil.copy(src_file, dest_file)
            else:
                print(f"Warning: {src_file} not found; skipping copy.")

def run_instance(instance_dir):
    """
    cd to instance_dir and run 'python3 tb_gene.py'.
    """
    cmd = "python3 tb_gene.py"
    subprocess.run(cmd, shell=True, cwd=instance_dir, check=True)

def update_final_file(instance_id, instance_dir, final_file):
    re_file = os.path.join(instance_dir, 're.txt')
    with open(final_file, 'a') as f_out:
        if os.path.exists(re_file):
            with open(re_file, 'r') as f:
                content = f.read()
            f_out.write(f"E1_{instance_id}:\n{content}\n")
        else:
            f_out.write(f"E1_{instance_id}: re.txt not found.\n")

def main():
    #  1) Hard-code the parent directory where your original files are located:
    parent_dir = ".../gain"

    #  2) The same directory is used as the 'source_dir' for copying files:
    source_dir = parent_dir

    #  3) The number of instance folders to create: 20 instances
    num_folders = 20

    #  4) The files to copy from source_dir to each E1_{i} folder:
    all_files = [
        "bubble.h",
        "kernel.cpp",
        "compile.py",
        "c_testbench.cpp",
        "tb_gene.py",
        "emb.py",
        "1.dat"
    ]

    #  5) The final results file:
    final_output_file = os.path.join(parent_dir, "final.txt")
    # Clear the final.txt file to ensure that real-time updates start with empty files
    with open(final_output_file, 'w') as f:
        f.write("")

    #  Create E1_1..E1_{num_folders} and copy files (no file modifications)
    create_folders_and_copy_files(source_dir, parent_dir, num_folders, all_files)

    # Randomly select the number of instances with injected errors: Randomly select a number from {2,3,4}
    error_inject_count = random.choice([2, 3, 4])
    # Randomly extract error_inject_count instances from 1 to num_folders for error injection
    error_inject_instances = random.sample(range(1, num_folders + 1), error_inject_count)
    print(f"The instance number of the injection error is: {error_inject_instances}")
    
    # Write error injection file inject_error.txt in the selected instance
    injection_message = "deliberate error injection for testing, at least 2errors"
    for i in error_inject_instances:
        instance_dir = os.path.join(parent_dir, f"E1_{i}")
        inject_file = os.path.join(instance_dir, "inject_error.txt")
        with open(inject_file, 'w') as f:
            f.write(injection_message)

    #  Run each instance and update final.txt immediately after each run
    for i in range(1, num_folders + 1):
        instance_dir = os.path.join(parent_dir, f"E1_{i}")
        print(f"Running instance: E1_{i}")
        run_instance(instance_dir)
        update_final_file(i, instance_dir, final_output_file)

    # Summary of results for all instances (optional)
    compilation_success = 0
    compilation_failed = 0
    simulation_success = 0
    simulation_failed = 0

    for i in range(1, num_folders + 1):
        instance_dir = os.path.join(parent_dir, f"E1_{i}")
        re_file = os.path.join(instance_dir, 're.txt')
        if os.path.exists(re_file):
            with open(re_file, 'r') as f:
                content = f.read()
            if "Compilation successfully" in content:
                compilation_success += 1
            else:
                compilation_failed += 1
            if "Simulation successfully" in content:
                simulation_success += 1
            else:
                simulation_failed += 1
        else:
            compilation_failed += 1
            simulation_failed += 1

    summary = (
        f"\nSummary:\n"
        f"Compilation successfully: {compilation_success}\n"
        f"Compilation failed: {compilation_failed}\n"
        f"Simulation successfully: {simulation_success}\n"
        f"Simulation failed: {simulation_failed}\n"
    )
    with open(final_output_file, 'a') as f_out:
        f_out.write(summary)
    
    print(summary)

if __name__ == "__main__":
    main()
