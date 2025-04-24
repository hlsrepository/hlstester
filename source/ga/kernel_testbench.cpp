# include <iostream>
# include <vector>
# include <algorithm>
# include <fstream>
# include <sstream>
using namespace std;

extern int ga(const vector<int>&, const vector<int>&, int&, int&, long long&, long long&);
extern int recursiveDepth(const vector<int>&, int);
void processLine( const vector<int>&, ofstream&, ofstream&, ofstream&);


int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input_file_path> <output_directory>" << endl;
        return 1;
    }

    ifstream input_file_path(argv[1]);
    string line;
    vector<pair<vector<int>, vector<int>>> testCases;
    string output_dir = argv[2];
    // Opening separate files for each variable in the specified output directory
    ofstream size1File(output_dir + "/size1.txt");
    ofstream size2File(output_dir + "/size2.txt");
    ofstream best1File(output_dir + "/best1.txt");
    ofstream best2File(output_dir + "/best2.txt");
    ofstream resultFile(output_dir + "/result.txt");
    ofstream divisionFile(output_dir + "/division1.txt");
    ofstream depthFile(output_dir + "/itera.txt");
    ofstream sumFile(output_dir + "/sum.txt");

    while (getline(input_file_path, line)) {
        istringstream iss(line);
        vector<int> nums1, nums2;
        int n;
        while (iss >> n) {
            nums1.push_back(n);
            if (iss.peek() == ',') iss.ignore();
        }
        if (!getline(input_file_path, line)) break;
        istringstream iss2(line);
        while (iss2 >> n) {
            nums2.push_back(n);
            if (iss2.peek() == ',') iss2.ignore();
        }
        testCases.emplace_back(nums1, nums2);
    }

    input_file_path.close(); // Close the input file after reading

    for (size_t i = 0; i < testCases.size(); ++i) {
        int size1, size2;
        long long best1, best2;
        int result = ga(testCases[i].first, testCases[i].second, size1, size2, best1, best2);

        // Process each line for recursive sum and division
        processLine(testCases[i].first, depthFile, divisionFile, sumFile);
        processLine(testCases[i].second, depthFile, divisionFile, sumFile);

        // Writing to different files
        size1File << size1 << endl;
        size2File << size2 << endl;
        best1File << best1 << endl;
        best2File << best2 << endl;
        resultFile << result << endl;
    }

    // Closing all output files
    size1File.close();
    size2File.close();
    best1File.close();
    best2File.close();
    resultFile.close();
    divisionFile.close();
    depthFile.close();
    sumFile.close();

    return 0;
}
