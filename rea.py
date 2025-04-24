import os
import openai

# File configuration
base_path = ".../hlsollm"
co_file = ".../kernel.cpp"
reasoning_path = os.path.join(base_path, "reasoning")

# Ensure directories exist
os.makedirs(reasoning_path, exist_ok=True)

# File reading functions
def read_file_content(file_path):
    try:
        with open(file_path, "r") as f:
            return f.read()
    except Exception as e:
        return f"Error reading {file_path}: {e}"

# Improved prompt generation clearly matching analysis steps
def generate_prompt(step, previous_analysis, extra_file_path=None):
    kernel_content = read_file_content(os.path.join(base_path, "kernel.cpp"))
    co_content = read_file_content(co_file)

    if step == 1:
        prompt = ("Step 1: Analyze the code for an initial understanding of the program structure.\n"
                  f"C/C++ Program:\n{co_content}\n\n"
                  f"HLS Program:\n{kernel_content}\n\n"
                  "Identify potential discrepancies clearly, such as Overflow, Truncation, Unexpected Address Access, Parallel Execution, Loop Unrolling Issues, Recursive Stack Overflow, Iteration Limits and so on.")

    elif step == 2:
        prompt = ("Step 2: Statement Analysis for Logic Understanding.\n"
                  "Based on the key variables pinpointed by the prior backward slicing, conduct an in-depth statement-level analysis to understand the logic flow.")

    elif step == 3:
        prompt = ("Step 3: Directive Analysis (#pragma) for Specific Interpretation.\n"
                  f"HLS Program:\n{kernel_content}\n\n"
                  " Review the #pragma or other directives in the HLS code to assess their influence on parallelism, memory access, etc. Identify what, where, and why potential discrepancies occur in HLS")
    return prompt

# Query GPT function clearly structured
def query_gpt(prompt, image_path=None, common=None):
    client = openai.OpenAI()
    system_message = "You are a helpful assistant for detecting discrepancies between C and HLS programs."

    if common:
        system_message += f"\nCommon discrepancies:\n{common}"

    messages = [
        {"role": "system", "content": system_message},
        {"role": "user", "content": prompt}
    ]

    response = client.chat.completions.create(
        model="gpt-4o",
        messages=messages
    )

    return response.choices[0].message.content

# History saving clearly separated for maintainability
def save_history(round_number, step, prompt, response):
    round_folder = os.path.join(reasoning_path, f"round_{round_number}")
    os.makedirs(round_folder, exist_ok=True)

    with open(os.path.join(round_folder, f"step_{step}_prompt.txt"), "w") as f:
        f.write(prompt)

    with open(os.path.join(round_folder, f"step_{step}_response.txt"), "w") as f:
        f.write(response)

    print(f"Saved prompt and response for round {round_number}, step {step}.")

# Main function
def main():
    rounds = 2
    common_discrepancies = ()

    for round_number in range(1, rounds + 1):
        print(f"\n--- Starting Analysis Round {round_number} ---")
        analysis_history = ""

        # Step 1: Code analysis
        prompt1 = generate_prompt(1, analysis_history)
        response1 = query_gpt(prompt1, common=common_discrepancies)
        save_history(round_number, 1, prompt1, response1)
        analysis_history += f"Code Analysis:\n{response1}\n\n"

        # Step 2: Statement analysis
        prompt2 = generate_prompt(2, analysis_history)
        response2 = query_gpt(prompt2, common=common_discrepancies)
        save_history(round_number, 2, prompt2, response2)
        analysis_history += f"Statement Analysis:\n{response2}\n\n"

        # Step 3: Directive analysis
        prompt3 = generate_prompt(3, analysis_history)
        response3 = query_gpt(prompt3, common=common_discrepancies)
        save_history(round_number, 3, prompt3, response3)

        print(f"--- Completed Analysis Round {round_number} ---")

if __name__ == "__main__":
    main()