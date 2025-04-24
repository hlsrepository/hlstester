import random
import copy
import sys
import os
import re

# Static variable definition, used to select a variant type (represented as a string)
enabled_mutations = "578"  



# Get input and output file paths
seed_file = sys.argv[1]  
output_file = sys.argv[2]  

# Define files and directories
base_dir = ".../hlsollm_modal"
check_dir = os.path.join(base_dir, "check")
spectre_dir = os.path.join(base_dir, "spectre")
probability_file = os.path.join(check_dir, "probability.txt")
mutation_log_file = os.path.join(check_dir, "mutation.txt")

# Create the required directory
os.makedirs(check_dir, exist_ok=True)
os.makedirs(spectre_dir, exist_ok=True)

# Global mutation number
global_mutation_index = 1
alpha = 0.04  


# Initialize the mutation number
def initialize_mutation_index():
    global global_mutation_index
    if os.path.exists(mutation_log_file):
        with open(mutation_log_file, 'r') as log_file:
            for line in log_file:
                match = re.match(r"Mutation (\d+):", line)
                if match:
                    last_index = int(match.group(1))
                    global_mutation_index = last_index + 1

# Here is a possible range of integers for use when inserting random numbers.
INT32_MIN = -2**12
INT32_MAX =  2**12 - 1

def check_and_fix_range(data):
    if isinstance(data, list):
        for i in range(len(data)):
            if isinstance(data[i], int):
                # Guaranteed to be within the range [INT32_MIN, INT32_MAX]
                data[i] = max(INT32_MIN, min(INT32_MAX, data[i]))
            elif isinstance(data[i], float):
                # Floating point range checking can do similar logic
                # Here is a brief demonstration. If you need to cut it, you can follow the actual needs.
                if data[i] < INT32_MIN:
                    data[i] = float(INT32_MIN)
                if data[i] > INT32_MAX:
                    data[i] = float(INT32_MAX)
    return data

#  1) Data Size Mutation
def mutate_size(data):
    if isinstance(data, list):
        data = copy.deepcopy(data)
        if random.choice([True, False]):
            # Insert random elements
            data.insert(random.randint(0, len(data)), random.randint(INT32_MIN, INT32_MAX))
        elif len(data) > 0:
            # Delete random elements
            data.pop(random.randint(0, len(data) - 1))
        data = check_and_fix_range(data)
    return data

#  2) Data Dimension Mutation
def mutate_dimension(data):
    if isinstance(data, list) and all(isinstance(row, list) for row in data):
        data = copy.deepcopy(data)
        # Randomly decide whether to add or delete a column
        if random.choice([True, False]):
            # Delete a column (if each row has at least 1 column)
            if all(len(row) > 0 for row in data):
                col_to_remove = random.randint(0, len(data[0]) - 1)
                for row in data:
                    row.pop(col_to_remove)
        else:
            # Add a column
            for row in data:
                row.append(random.randint(INT32_MIN, INT32_MAX))

        # Check and correct the data range per row
        for idx, row in enumerate(data):
            data[idx] = check_and_fix_range(row)
    return data

#  3) Zero Value Mutation
def mutate_zero(data):

    if isinstance(data, list) and len(data) > 0:
        data = copy.deepcopy(data)
        idx = random.randint(0, len(data) - 1)
        data[idx] = 0
        data = check_and_fix_range(data)
    return data

#  4) Order Mutation
def mutate_order(data):

    if isinstance(data, list):
        data = copy.deepcopy(data)
        random.shuffle(data)
        data = check_and_fix_range(data)
    return data

#  5) Data Element Mutation
def mutate_element(data):

    if isinstance(data, list) and len(data) > 0:
        data = copy.deepcopy(data)
        idx = random.randint(0, len(data) - 1)
        # Randomly change values ​​within ±5
        delta = random.randint(-5, 5)
        if isinstance(data[idx], int):
            data[idx] += delta
        elif isinstance(data[idx], float):
            data[idx] += float(delta)
        data = check_and_fix_range(data)
    return data

#  6) Data Type Mutation
def mutate_type(data):

    if isinstance(data, list) and len(data) > 0:
        data = copy.deepcopy(data)
        idx = random.randint(0, len(data) - 1)
        if isinstance(data[idx], int):
            # Integer -> Floating point number
            data[idx] = float(data[idx])
        elif isinstance(data[idx], float):
            # Floating point number -> integer
            data[idx] = int(data[idx])
        data = check_and_fix_range(data)
    return data

#  7) Bit Mutation
def mutate_bit(data):

    if isinstance(data, list) and len(data) > 0:
        data = copy.deepcopy(data)
        idx = random.randint(0, len(data) - 1)
        if isinstance(data[idx], int) and data[idx] != 0:
            bit_length = data[idx].bit_length()
            if bit_length > 0:
                bit_pos = random.randint(0, bit_length - 1)
                data[idx] ^= (1 << bit_pos)
        data = check_and_fix_range(data)
    return data

#  8) Byte Mutation
def mutate_byte(data):

    if isinstance(data, list) and len(data) > 0:
        data = copy.deepcopy(data)
        idx = random.randint(0, len(data) - 1)
        if isinstance(data[idx], int):
            # Select a byte bit to flip (0~3)
            byte_pos = random.randint(0, 3)
            mask = 0xFF << (byte_pos * 8)
            data[idx] ^= mask
        data = check_and_fix_range(data)
    return data

# Mapping table of variant types and numbers
mutation_map = {
    1: mutate_size,         #  Data Size Mutation
    2: mutate_dimension,    #  Data Dimension Mutation
    3: mutate_zero,         #  Zero Value Mutation
    4: mutate_order,        #  Order Mutation
    5: mutate_element,      #  Data Element Mutation
    6: mutate_type,         #  Data Type Mutation
    7: mutate_bit,          #  Bit Mutation
    8: mutate_byte,         #  Byte Mutation
}

# Mutation methods enabled based on static variable parsing
enabled_mutation_funcs = [mutation_map[int(m)] for m in enabled_mutations if int(m) in mutation_map]
# Initialize weights, equal weights
mutation_weights = {func.__name__: 1 / len(enabled_mutation_funcs) for func in enabled_mutation_funcs}


def read_seed_file():
    with open(seed_file, 'r') as f:
        return [[int(num) for num in line.strip().split()] for line in f]

def write_mutated_file(data):
    with open(output_file, 'w') as f:
        for line in data:
            f.write(' '.join(map(str, line)) + '\n')

def log_mutation(index, original, mutated, mutation_type, source_file, source_index, priority=False, log_used=False):
    """
    Record the information of each mutation to mutation.txt.
    :param index: Current mutation number.
    :param original: Original test case.
    :param mutated: Mutated test case.
    :param mutation_type: Mutation type.
    :param source_file: Original data source file.
    :param source_index: Line number of original data in the file.
    :param priority: Whether it is a priority test case (True is priority, False is random).
    :param log_used: Whether it is an actual mutation used. If False, it will not be written to mutation.txt.
    """
    if not log_used:
        return  

    global global_mutation_index
    priority_str = "Priority Case" if priority else "Random Case"
    with open(mutation_log_file, 'a') as log_file:
        log_file.write(f"Mutation {global_mutation_index} ({priority_str}):\n")
        log_file.write(f"Source File: {source_file}, Index: {source_index}\n")
        log_file.write(f"Original: {original}\n")
        log_file.write(f"Mutated : {mutated}\n")
        log_file.write(f"Mutation Type: {mutation_type}\n\n")
    global_mutation_index += 1


    """
    Generate mutation data based on seed data and record the mutation source.
    :param seed_data: Input seed data.
    :param source_file: File name of the data source.
    :param start_index: The starting number of the data in the source file.
    """
# Use static variable control to enable mutation methods
# Modify the generated mutant data function to detect and regenerate
def generate_mutations(seed_data, source_file, start_index=1):
    mutation_funcs = enabled_mutation_funcs
    mutated_data = []
    mutation_tracking = []

    for index, line in enumerate(seed_data, start=start_index):
        original_line = copy.deepcopy(line)
        
        # Randomly select an enabled mutation method
        mutation_func = random.choices(
            enabled_mutation_funcs,
            weights=[mutation_weights[func.__name__] for func in enabled_mutation_funcs],
            k=1
        )[0]

        mutated_line = mutation_func(copy.deepcopy(line))
        
        # Check and regenerate until the scope is met
        while any(value < INT32_MIN or value > INT32_MAX for value in mutated_line):
            mutated_line = mutation_func(copy.deepcopy(line))
        
        mutated_data.append(mutated_line)

        # Record mutation information
        mutation_type = mutation_func.__name__
        log_mutation(
            index, original_line, mutated_line, mutation_type, 
            source_file, index, priority=False  
        )
        
        # Record mutation tracking information
        mutation_tracking.append((index, mutation_type))
    
    return mutated_data, mutation_tracking



def check_priority_cases(current_round, mutation_tracking):
    priority_cases = {}  
    priority_spec_info = []

    for fname in ["input_a_sp.txt", "input_b_sp.txt", "add_result_sp.txt", "subtract_result_sp.txt", "divide_sp.txt", "multiply_sp.txt", "bitwise_and_sp.txt", "bitwise_or_sp.txt",  "bitwise_xor_sp.txt", "mod_result_sp.txt"]:
        path = os.path.join(spectre_dir, fname)
        
        # Check if the file exists
        if not os.path.exists(path):
            print(f"Warning: {path} not found. Skipping this file.")
            continue
        
        # Read all values ​​of the current round file and find the global maximum
        with open(path, 'r') as file:
            lines = [float(line.split()[1]) for line in file.readlines()]
        
        # Make sure that the index of mutation_tracking is not out of range
        min_length = min(len(lines), len(mutation_tracking))
        
        # Determine priority test cases and record maximum or minimum values
        max_val, min_val = max(lines), min(lines)
        for idx, val in enumerate(lines[:min_length]):
            if val == max_val or val == min_val:
                test_vector_id = idx + 1  
                variable = fname.split('_')[0] 
                mutation_type = mutation_tracking[idx][1]  
                status = "Max" if val == max_val else "Min"

                # Update priority test cases
                if test_vector_id not in priority_cases:
                    priority_cases[test_vector_id] = []
                priority_cases[test_vector_id].append((variable, status, mutation_type))

    # Organize the details of priority test cases
    for test_vector_id, details in priority_cases.items():
        try:
            details_str = "; ".join([f"{var} ({status}, {mutation_type})" for var, status, mutation_type in details])
        except ValueError:
            print(f"Malformed details for Test Vector {test_vector_id}: {details}")
            continue
        priority_spec_info.append(f"Test Vector {test_vector_id}: {details_str}")
    
    return priority_cases, priority_spec_info


def update_mutation_probabilities(priority_cases, mutation_tracking):
    global mutation_weights
    affected_mutations = set(
        mutation_tracking[case - 1][1] for case in priority_cases if case <= len(mutation_tracking)
    )

    # Update weight: Increase triggered mutations and reduce untriggered mutations
    for func_name in mutation_weights:
        if func_name in affected_mutations:
            mutation_weights[func_name] += alpha  
        else:
            mutation_weights[func_name] -= alpha / (len(mutation_weights) - 1)  

    # Normalized weights
    total_weight = sum(max(w, 0) for w in mutation_weights.values())  
    for func_name in mutation_weights:
        mutation_weights[func_name] = max(mutation_weights[func_name], 0) / total_weight


def generate_next_dat_file(priority_cases, seed_data, mutate_ratio, mutation_file):
    next_data = []
    mutation_tracking = []
    total_test_cases = len(seed_data)
    required_mutation_count = int(total_test_cases * mutate_ratio)

    # Record the source information of the mutation
    priority_vectors = [(i, seed_data[i - 1]) for i in priority_cases if i <= len(seed_data)]
    
    # Handle priority test cases
    for idx, (vector_id, vector) in enumerate(priority_vectors[:required_mutation_count], start=1):
        mutated_vector, tracking_info = generate_mutations([vector], source_file=seed_file, start_index=vector_id)
        next_data.extend(mutated_vector)
        mutation_tracking.extend(tracking_info)
        # Logging priority test cases
        log_mutation(
            idx, vector, mutated_vector[0], tracking_info[0][1],
            seed_file, vector_id, priority=True, log_used=True
        )

    # Supplement the remaining random test cases
    if len(next_data) < required_mutation_count:
        remaining_needed = required_mutation_count - len(next_data)
        random_samples = random.sample(list(enumerate(seed_data, start=1)), remaining_needed)
        for idx, (vector_id, vector) in enumerate(random_samples, start=1):
            mutated_vector, tracking_info = generate_mutations([vector], source_file=seed_file, start_index=vector_id)
            next_data.extend(mutated_vector)
            mutation_tracking.extend(tracking_info)
            # Randomly selected test cases for logging
            log_mutation(
                idx, vector, mutated_vector[0], tracking_info[0][1],
                seed_file, vector_id, priority=False, log_used=True
            )

    # Ensure the correct number of variations
    assert len(next_data) == required_mutation_count, \
        f"Expected {required_mutation_count} mutated cases, but got {len(next_data)}"

    # Save the mutated data to the file
    with open(mutation_file, 'w') as mutation_f:
        for mutation in next_data:
            mutation_f.write(' '.join(map(str, mutation)) + '\n')

    return next_data

def extract_round_number(file_name):
    base_name = file_name.split('/')[-1]  
    round_match = re.match(r"(\d+)", base_name)  
    if round_match:
        return int(round_match.group(1))
    else:
        raise ValueError(f"Invalid file name format: {file_name}")


def main():
    # Print enabled variant types and functions
    print(f"Enabled Mutations: {enabled_mutations}")
    print(f"Mutation Functions: {[func.__name__ for func in enabled_mutation_funcs]}")

    try:
        # Extract the current round number
        current_round = extract_round_number(output_file) - 1
    except ValueError as e:
        print(f"Error extracting round number: {e}")
        sys.exit(1)

    # Define the priority test case files and specification files for the current round
    priority_file = os.path.join(check_dir, f"priority_{current_round}.txt")
    priority_spec_file = os.path.join(check_dir, f"priority_{current_round}_spec.txt")

    # Initialize the mutation number
    initialize_mutation_index()

    # Read seed file data
    seed_data = read_seed_file()

    # Generate mutation data and mutation tracking information
    mutated_data, mutation_tracking = generate_mutations(seed_data, seed_file)

    # Write mutated data to output file
    write_mutated_file(mutated_data)

    # Check priority test cases and their specification information
    priority_cases, priority_spec_info = check_priority_cases(current_round, mutation_tracking)

    # Save priority test case information to file
    with open(priority_file, 'w') as f:
        if priority_cases:
            for test_vector_id, details in priority_cases.items():
                details_str = "; ".join([f"{var} ({status}, {mutation_type})" for var, status, mutation_type in details])
                f.write(f"Test Vector {test_vector_id}: {details_str}\n")
        else:
            f.write("No priority cases in this round.\n")

    # Save detailed specification information for priority test cases to file
    with open(priority_spec_file, 'w') as f:
        if priority_spec_info:
            for line in priority_spec_info:
                f.write(line + "\n")
        else:
            f.write("No priority cases in this round.\n")

    # Update mutation weights
    update_mutation_probabilities(priority_cases.keys(), mutation_tracking)

    # Output updated mutation weight
    print(f"Updated Mutation Weights: {mutation_weights}")

    # Save the mutation weight to the file
    with open(probability_file, 'a') as f:
        f.write(f"Round {current_round}: {mutation_weights}\n")

    # Get the mutation ratio (default is 0.4)
    mutate_ratio = float(sys.argv[3]) # if len(sys.argv) > 3 else 0.3

    # Generate the next round of test data and save it to a file
    mutation_file = output_file.replace(".dat", "_mutation.dat")
    next_data = generate_next_dat_file(priority_cases, seed_data, mutate_ratio, mutation_file)

    # Write the next round of mutation data generated
    write_mutated_file(next_data)

    # Print the final generated file information
    print(f"Mutated data written to: {output_file}")
    print(f"Mutation records saved to: {mutation_file}")
    print(f"Priority cases saved to: {priority_file}")
    print(f"Priority specifications saved to: {priority_spec_file}")
    print(f"Mutation probabilities saved to: {probability_file}")


if __name__ == "__main__":
    main()

