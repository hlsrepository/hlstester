import os
import openai

#  Configurations
base_path = ".../hlsollm_modal"
testbench_file_path = os.path.join(base_path, "kernel.cpp")
template_path = os.path.join(base_path, "temp.cpp")
instrumented_file_path = os.path.join(base_path, "instrument", "kernel_testbench_ins.cpp")
instrument_folder = os.path.join(base_path, "instrument")

def read_file_content(file_path):
    try:
        with open(file_path, "r") as file:
            return file.read()
    except Exception as e:
        return f"Error reading {file_path}: {e}"

def write_file_content(file_path, content):
    try:
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
        with open(file_path, "w") as file:
            file.write(content)
        print(f"File saved: {file_path}")
    except Exception as e:
        print(f"Error writing to {file_path}: {e}")

def extract_info_from_response(history_path, step):
    response_path = os.path.join(history_path, f"step_{step}_response.txt")
    try:
        with open(response_path, "r") as file:
            return file.read().strip()
    except FileNotFoundError:
        return ""

def extract_code_from_response(response):
    #  Extract the code part from the GPT response enclosed in triple backticks with cpp language marker
    marker = "```cpp"
    end_marker = "```"
    start = response.find(marker)
    if start != -1:
        start += len(marker)  #  move start index right after the marker to skip 'cpp'
        end = response.find(end_marker, start)
        if end != -1:
            return response[start:end].strip()  #  extract the code between ```cpp and the next ```
    return "No code block found."


def instrument_code(modules, variables, code_content, template_content):
    client = openai.OpenAI()
    prompt = (f"Given the code for a kernel test bench and lists of key variables, instrument the following testbench code and save the key variables spectra in .txt files.\n\n"
              f"The sensitive variables are: \n{', '.join(variables)}.\n\n"
              f"The code to be instrumented is as follows:\n{code_content}\n\n"
              f"You can refer to the instrumentation template. Please note that you do not refer to the specific code logic below, but only refer to the instrumentation method and file saving method::\n{template_content}\n\n")

    response = client.chat.completions.create(
        model="gpt-4o",
        messages=[{"role": "user", "content": prompt}]
    )
    response_text = response.choices[0].message.content

    #  Save prompt and response
    write_file_content(os.path.join(instrument_folder, f"prompt.txt"), prompt)
    write_file_content(os.path.join(instrument_folder, f"response.txt"), response_text)

    return extract_code_from_response(response_text)

def main():
    reasoning_path = os.path.join(base_path, "reasoning", "1")
    code_content = read_file_content(testbench_file_path)
    template_code = read_file_content(template_path)
    
    if not template_code:
        print("No template code found, aborting.")
        return

    modules = extract_info_from_response(reasoning_path, 2).split(", ")
    variables = extract_info_from_response(reasoning_path, 3).split(", ")

    instrumented_code = instrument_code(modules, variables, code_content, template_code)
    write_file_content(instrumented_file_path, instrumented_code)

if __name__ == "__main__":
    main()
