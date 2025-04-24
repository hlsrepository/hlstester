# include <iostream>
# include <fstream>
# include <string>
# include "bubble.h" // This file defines addAndDivide and addAndMultiply

// Helper function: write result to file
void saveResult(std::ofstream& file, int result) {
    if (file.is_open()) {
        file << result << std::endl;
    } else {
        std::cerr << "Unable to write to result file." << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] 
                  << " <input_file_path> <output_directory> <iteration_number>" 
                  << std::endl;
        return 1;
    }

    const std::string input_file_path = argv[1];
    const std::string output_dir = argv[2];
    const std::string iteration_number = argv[3]; // Iteration number (not used in this example, can be extended as needed)

    // Construct output file paths
    std::string output_base = output_dir;

    std::ifstream inputFile(input_file_path);
    std::ofstream divideOutputFile(output_base + "/divideResult.txt");
    std::ofstream multiplyOutputFile(output_base + "/multiplyResult.txt");
    std::ofstream addResultFile(output_base + "/a+b.txt");
    std::ofstream subtractResultFile(output_base + "/a-b.txt");
    std::ofstream andResultFile(output_base + "/a&b.txt");
    std::ofstream orResultFile(output_base + "/a|b.txt");
    std::ofstream xorResultFile(output_base + "/a^b.txt");
    std::ofstream modResultFile(output_base + "/a%b.txt");
    std::ofstream inputAFile(output_base + "/a.txt");
    std::ofstream inputBFile(output_base + "/b.txt");

    // Check if input and output files were opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open input file: " << input_file_path << std::endl;
        return 1;
    }
    if (!divideOutputFile.is_open() || !multiplyOutputFile.is_open() || 
        !addResultFile.is_open() || !subtractResultFile.is_open() || 
        !andResultFile.is_open() || !orResultFile.is_open() || 
        !xorResultFile.is_open() || !modResultFile.is_open() || 
        !inputAFile.is_open() || !inputBFile.is_open()) {
        std::cerr << "Unable to open output files." << std::endl;
        return 1;
    }

    int a, b;
    while (inputFile >> a >> b) {
        // Save input data
        inputAFile << a << std::endl;
        inputBFile << b << std::endl;

        // Call two functions separately for computation
        int divideResult = addAndDivide(a, b);
        int multiplyResult = addAndMultiply(a, b);

        // Save each variable's result (using global variables for intermediate results)
        saveResult(divideOutputFile, divideResult);
        saveResult(multiplyOutputFile, multiplyResult);
        saveResult(addResultFile, add_result);
        saveResult(subtractResultFile, subtract_result);
        saveResult(andResultFile, bitwise_and_result);
        saveResult(orResultFile, bitwise_or_result);
        saveResult(xorResultFile, bitwise_xor_result);
        saveResult(modResultFile, mod_result);
    }

    // Close all files
    inputFile.close();
    divideOutputFile.close();
    multiplyOutputFile.close();
    addResultFile.close();
    subtractResultFile.close();
    andResultFile.close();
    orResultFile.close();
    xorResultFile.close();
    modResultFile.close();
    inputAFile.close();
    inputBFile.close();

    std::cout << "Test completed. Results have been saved to: " << output_base << std::endl;
    return 0;
}
