# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>

extern void dfs(int, int*, int*, std::vector<int>&, int*, int*, int*, int*);

struct TestCase {
    std::vector<int> input;
};

void saveResultsArray(std::ofstream& file, const std::vector<int>& results) {
    if (file.is_open()) {
        for (const auto& result : results) {
            file << result << " ";  
        }
        file << std::endl;  
    } else {
        std::cerr << "Cannot write to file." << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file_path> <output_directory>" << std::endl;
        return 1;
    }

    std::ifstream input_file_path(argv[1]);
    std::string line;
    std::vector<TestCase> testCases;

    std::string output_dir = argv[2];
    // Opening separate files for each variable in the specified output directory
    std::ofstream sizeFile(output_dir + "/size.txt");
    std::ofstream feedbackFile(output_dir + "/feedback.txt");
    std::ofstream recursionFile(output_dir + "/recursion.txt");
    std::ofstream diffFile(output_dir + "/diff.txt");
    std::ofstream resultFile(output_dir + "/result.txt");
    std::ofstream feedbackOutputFile(output_dir + "/feedbackOutput.txt");
    std::ofstream input0File(output_dir + "/input0.txt");
    std::ofstream input1File(output_dir + "/input1.txt");
    std::ofstream input2File(output_dir + "/input2.txt");
    std::ofstream input3File(output_dir + "/input3.txt");

    while (getline(input_file_path, line)) {
        std::istringstream iss(line);
        TestCase tc;
        int temp;

        while (iss >> temp) {
            tc.input.push_back(temp);
        }

        testCases.push_back(tc);
    }

    input_file_path.close(); // Close the input file after reading

    for (int i = 0; i < testCases.size(); i++) {
        std::vector<int> results(testCases[i].input.size(), 0);
        std::vector<int> feedbackOutput(testCases[i].input.size(), 0);
        int fallback = 0, feedbackValue = 0, recursionResult = 0, diff = 0;

        dfs(testCases[i].input.size(), testCases[i].input.data(), results.data(), feedbackOutput, &fallback, &feedbackValue, &recursionResult, &diff);

        // Writing to different files
        sizeFile << testCases[i].input.size() << std::endl;
        feedbackFile << feedbackValue << std::endl;
        recursionFile << recursionResult << std::endl;
        diffFile << diff << std::endl;
        saveResultsArray(resultFile, results);
        saveResultsArray(feedbackOutputFile, feedbackOutput);
        input0File << (testCases[i].input.empty() ? 0 : testCases[i].input[0]) << std::endl;
        input1File << (testCases[i].input.empty() ? 0 : testCases[i].input[1]) << std::endl;
        input2File << (testCases[i].input.empty() ? 0 : testCases[i].input[2]) << std::endl;
        input3File << (testCases[i].input.empty() ? 0 : testCases[i].input[3]) << std::endl;
    }

    // Closing all output files
    sizeFile.close();
    feedbackFile.close();
    recursionFile.close();
    diffFile.close();
    resultFile.close();
    feedbackOutputFile.close();
    input0File.close();
    input1File.close();
    input2File.close();
    input3File.close();

    return 0;
}
