import subprocess
import os
import openai
import shutil
import time
import re
import sys

# --------------------------------------------------------------------------------------------------------------------------------
# The directory where the current script is located is the instance directory
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
        # The hls_exec parameter is set to 2: execute csynth_design and cosim_design
        f.write('set hls_exec 2\n')
        f.write(f'set kernel_path "{os.path.join(base_dir, "1.dat")}"\n')
        f.write(f'set output_dir "{os.path.join(base_dir, "p")}"\n')
        f.write(f'csim_design -argv "$kernel_path $output_dir"\n')
        f.write('if {$hls_exec == 1} {\n')
        f.write('    csynth_design\n')
        f.write('} elseif {$hls_exec == 2} {\n')
        f.write('    csynth_design\n')
        f.write('    cosim_design -argv "$kernel_path $output_dir"\n')
        f.write('} elseif {$hls_exec == 3} {\n')
        f.write('    csynth_design\n')
        f.write('    cosim_design -argv "$kernel_path $output_dir"\n')
        f.write('    export_design\n')
        f.write('} else {\n')
        f.write('    csynth_design\n')
        f.write('}\n')
        f.write('quit\n')
    return tcl_script_path

def run_tcl_script(tcl_script_path, vitis_bin):
    """Run the TCL script of Vitis HLS and count it, returning the run time and output text"""
    command = f'{vitis_bin}/vitis-run --mode hls --tcl {tcl_script_path}'
    start_time = time.time()
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    end_time = time.time()
    elapsed_time = end_time - start_time
    return elapsed_time, result.stdout

def run_hls(kernel_path, testbench_path, base_dir, vitis_bin):
    """Call the Vitis HLS process, generate the TCL script and run it, returning the execution time and output"""
    input_file_path = os.path.join(base_dir, "1.dat")
    output_dir = os.path.join(base_dir, "p")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    tcl_script_path = create_vitis_tcl_script(kernel_path, testbench_path, [], input_file_path, output_dir)
    elapsed_time, output_text = run_tcl_script(tcl_script_path, vitis_bin)
    return elapsed_time, output_text

# --------------------------------------------------------------------------------------------------------------------------------

def main_instance(base_dir, enabled):
    emb_script_path = os.path.join(base_dir, "emb.py")
    library_dir = ".../autolook"
    
    kernel_file = os.path.join(base_dir, "kernel.cpp")
    # The first round of generated testbench file is named kernel_testbench0.cpp
    testbench_file = os.path.join(base_dir, "kernel_testbench0.cpp")
    
    if not os.path.exists(kernel_file):
        raise FileNotFoundError(f"kernel.cpp does not exist in the instance directory: {kernel_file}")
    
    # --------------------------------------------------------------------------------------------------------------------------------
    print("The first round: Generate testbench code (only kernel.cpp content is provided)...")
    template_content = ""
    ref_code = ""
    # The first round prompt uses prompt0.txt; no error message is transmitted, the last round testbench code
    prompt0 = os.path.join(base_dir, "prompt0.txt")
    generated_code = consult_gpt("", prompt0, template_content, ref_code, prev_testbench_code="")
    # Save GPT's answer to GPT0.txt
    gpt0_file = os.path.join(base_dir, "GPT0.txt")
    with open(gpt0_file, 'w') as f:
        f.write(generated_code)
    # Extract code from GPT0.txt (extract
    # cpp to
    # Content between)
    new_code = extract_code_from_gpt_output(gpt0_file)
    with open(testbench_file, 'w') as f:
        f.write(new_code)
    print("kernel_testbench0.cpp has been generated.")
    
    # Check if an instance is injected by an error
    instance_name = os.path.basename(base_dir)
    inject_error_file = os.path.join(base_dir, "inject_error.txt")
    error_injection = ""
    if os.path.exists(inject_error_file):
        with open(inject_error_file, 'r') as f:
            error_injection = f.read().strip()
        print(f"The current instance {instance_name} is an error injected instance, and the injected error is: {error_injection}")
    
    current_testbench = testbench_file
    iteration_limit = 2
    previous_errors = ""
    vitis_bin = '.../bin'
    compile_output = ""
    
    # First round HLS comprehensive
    elapsed_time, compile_output = run_hls(kernel_file, current_testbench, base_dir, vitis_bin)
    print(f"The first round of HLS comprehensive time {elapsed_time:.2f} seconds")
    print("First round HLS output:")
    print(compile_output)
    previous_errors = extract_error_info(compile_output)
    
    # If the successful identification is matched in the compilation output, it will end directly
    if "INFO: [COSIM 212-1000] *** C/RTL co-simulation finished: PASS ***" in compile_output:
        print("Compilation is successful, stop iteratively modifying testbench!")
        correct_file_name = os.path.basename(kernel_file).replace('.cpp', '_correct.cpp')
        correct_file_path = os.path.join(base_dir, correct_file_name)
        shutil.copy(kernel_file, correct_file_path)
        print(f"Correct code output as {correct_file_name}")
        # Write re.txt before exiting
        re_file_path = os.path.join(base_dir, "re.txt")
        with open(re_file_path, 'w') as re_file:
            re_file.write("Compilation successfully, Simulation successfully.\n")
        return

    # Starting from the second round
    for iteration in range(1, iteration_limit):
        print(f"\nIteration {iteration + 1}: Generate a new testbench and perform HLS synthesis...")
        prompt_file = os.path.join(base_dir, f"prompt{iteration + 1}.txt")
        with open(current_testbench, 'r') as f:
            prev_testbench_code = f.read()
        errors = previous_errors  
        template_content = ""
        ref_code = ""
        if enabled in ["2", "3"]:
            # Pass the current testbench file when calling get_template
            template_file_name, template_content, highest_similarity = get_template(emb_script_path, library_dir, errors, current_testbench)
            if template_content:
                print(f"Template Name: {template_file_name}, Similarity: {highest_similarity}")
        if enabled == "3":
            ref_testbench_path = os.path.join(base_dir, "c_testbench.cpp")
            if os.path.exists(ref_testbench_path):
                with open(ref_testbench_path, 'r') as file:
                    ref_code = file.read()
            else:
                print(f"The referenced C++ testbench file not found: {ref_testbench_path}")
        
        fixed_code = consult_gpt(errors, prompt_file, template_content, ref_code, prev_testbench_code, error_injection)
        # Save the GPT output to GPT{i}.txt and extract the code from it
        gpt_output_file = os.path.join(base_dir, f"GPT{iteration + 1}.txt")
        with open(gpt_output_file, 'w') as f:
            f.write(fixed_code)
        new_code = extract_code_from_gpt_output(gpt_output_file)
        # The newly generated testbench is named kernel_testbench0_c{iteration}.cpp
        current_testbench = os.path.join(base_dir, f"kernel_testbench0_c{iteration}.cpp")
        with open(current_testbench, 'w') as f:
            f.write(new_code)
        print(f"A new testbench file was generated: {os.path.basename(current_testbench)}")
        
        elapsed_time, compile_output = run_hls(kernel_file, current_testbench, base_dir, vitis_bin)
        print(f"HLS comprehensive time {elapsed_time:.2f} seconds")
        print("HLS output:")
        print(compile_output)
        
        # Check whether the success flag is included
        if "INFO: [COSIM 212-1000] *** C/RTL co-simulation finished: PASS ***" in compile_output:
            print("Compilation is successful, stop iteratively modifying testbench!")
            correct_file_name = os.path.basename(kernel_file).replace('.cpp', '_correct.cpp')
            correct_file_path = os.path.join(base_dir, correct_file_name)
            shutil.copy(kernel_file, correct_file_path)
            print(f"Correct code output as {correct_file_name}")
            break
        
        errors = extract_error_info(compile_output)
        if not errors:
            print(f"Success: HLS synthesis has no errors after {iteration + 1} iterations.")
            correct_file_name = os.path.basename(kernel_file).replace('.cpp', '_correct.cpp')
            correct_file_path = os.path.join(base_dir, correct_file_name)
            shutil.copy(kernel_file, correct_file_path)
            print(f"Correct code output as {correct_file_name}")
            break
        else:
            previous_errors = errors
    # Write the final HLS output to re.txt
    re_file_path = os.path.join(base_dir, "re.txt")
    compilation_status = "Compilation failed"
    simulation_status = "Simulation failed"
    if "INFO: [SIM 211-1] CSim done with 0 errors" in compile_output:
        compilation_status = "Compilation successfully"
    if "INFO: [COSIM 212-1000] *** C/RTL co-simulation finished: PASS ***" in compile_output:
        simulation_status = "Simulation successfully"
    with open(re_file_path, 'w') as re_file:
        re_file.write(f"{compilation_status}, {simulation_status}.\n")
    print("Completed all iterations.")

def extract_error_info(compile_output):
    start_marker = "CSIM start"
    end_marker = "CSIM finish"
    start_idx = compile_output.find(start_marker)
    end_idx = compile_output.find(end_marker)
    if start_idx != -1 and end_idx != -1 and end_idx > start_idx:
        return compile_output[start_idx:end_idx]
    return compile_output

def consult_gpt(errors, prompt_file, template_content="", ref_code="", prev_testbench_code="", error_injection=""):
    """
    Call GPT to generate or correct HLS testbench code.
    Parameters:
    errors: Error information extracted from the compilation output (used in the second round and later). If it is empty, no error information will be added.
    prompt_file: File path for saving prompt text.
    template_content: External testbench template (optional).
    ref_code: Reference C++ testbench code (optional).
    prev_testbench_code: Complete code of the testbench generated in the previous round.
    error_injection: If it is not empty, it means that the current instance is an error injection instance, and this error information is attached to the GPT prompt.
    The kernel code is automatically read from kernel.cpp internally and added to the prompt.
    """
    kernel_path = os.path.join(base_dir, "kernel.cpp")
    try:
        with open(kernel_path, 'r') as f:
            kernel_code = f.read()
    except Exception as e:
        kernel_code = f"Unable to read kernel.cpp: {e}"
    
    message_content = "As a professional HLS programmer,"
    # If there is an error message, provide the error message first
    if errors:
        if isinstance(errors, list):
            error_text = "\n".join(errors)
        else:
            error_text = errors
        message_content += "The following is the error message of testbench when compiling the previous round of HLS code: \n" + error_text + "\n\n"
    # Add the testbench code generated in the previous round (from the second round)
    if prev_testbench_code:
        message_content += "The testbench code generated in the previous round is as follows:\n" + prev_testbench_code + "\n\n"
    message_content += "The kernel code of HLS is as follows:\n" + kernel_code
    if template_content:
        message_content += "\n\nExternal testbench template is as follows (for reference): \n" + template_content
    if ref_code:
        message_content += "\n\nThe referenced C++ testbench code is as follows (for further reference): \n" + ref_code
    message_content += "\n\nDo not modify the HLS kernel code, please carefully modify and provide a complete and successfully compiled HLS testbench code. Please make sure the output code is complete and valid."
    
    # Append error injection content to GPT prompt to the error injection instance, but not to write to the prompt file
    injected_error_text = ""
    if error_injection:
        injected_error_text = "\nINJECTED_ERROR: " + error_injection + "\n"
        # Print the current instance as an error injection instance (already printed in main_instance, but confirm again)
        print(f"[Error Injection] The current instance {os.path.basename(base_dir)} will inject the same error in all iterations.")
    
    # Generate the complete prompt sent to GPT (including error injection part)
    message_content_for_gpt = message_content + injected_error_text
    
    # Write prompt that does not contain the error injection part to prompt_file
    with open(prompt_file, 'w') as f:
        f.write(message_content)
    
    client = openai.OpenAI()
    response = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[
            {"role": "system", "content": "You are a helper program for generating and repairing HLS testbench code."},
            {"role": "user", "content": message_content_for_gpt}
        ]
    )
    return response.choices[0].message.content

def get_template(emb_script_path, library_dir, error_message, testbench_file):
    """
    Call emb.py for template matching.
    Parameters:
    error_message: error message (from extract_error_info())
    testbench_file: current updated testbench file (such as kernel_testbench0 or kernel_testbench0_c{iteration}.cpp)
    library_dir: the directory where the template is located
    The output format is "Template file name, similarity"
    """
    template_command = f"python3 {emb_script_path} '{error_message}' {testbench_file} {library_dir}"
    result = subprocess.run(template_command, shell=True, capture_output=True, text=True)
    if result.stdout.strip():
        output = result.stdout.strip().split(',')
        if len(output) >= 2:
            template_file_name = output[0].strip()
            highest_similarity = float(output[1].strip())
            template_file_path = os.path.join(library_dir, template_file_name)
            try:
                with open(template_file_path, 'r') as file:
                    template_content = file.read()
                print(f"Debug: Template content for {template_file_name} is {'empty' if not template_content else 'not empty'}")
                return template_file_name, template_content, highest_similarity
            except FileNotFoundError:
                print(f"File not found: {template_file_path}")
                return "", "", 0.0
    return "", "", 0.0

def extract_code_from_gpt_output(gpt_output_file):
    code_content = ""
    capture = False
    with open(gpt_output_file, 'r') as file:
        for line in file:
            if line.strip() == '```cpp':
                capture = True
                continue
            elif line.strip() == '```' and capture:
                break
            if capture:
                code_content += line
    return code_content

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--enabled", type=str, default="3", help="generate scheme: '1', '2', or '3'")
    args = parser.parse_args()
    main_instance(base_dir, args.enabled)
