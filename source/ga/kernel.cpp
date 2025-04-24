# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <algorithm>
using namespace std;

const int MOD = 1000000007; 

int recursiveDepth(const vector<int>& nums, int depth) {
    if (nums.empty()) return depth;
    if (nums.size() == 1) return depth + 1;
    vector<int> rest(nums.begin() + 1, nums.end());
    return recursiveDepth(rest, depth + 1);
}

int ga(const vector<int>& nums1, const vector<int>& nums2, int& size1, int& size2, long long& best1, long long& best2) {
    size1 = nums1.size();
    size2 = nums2.size();
    best1 = 0;
    best2 = 0;
    int i = 0, j = 0;

    while (i < size1 || j < size2) {
        if (i < size1 && j < size2) {
            if (nums1[i] < nums2[j]) {
                best1 += nums1[i];
                ++i;
            } else if (nums1[i] > nums2[j]) {
                best2 += nums2[j];
                ++j;
            } else {
                long long best = max(best1, best2) + nums1[i];
                best1 = best2 = best;
                ++i;
                ++j;
            }
        } else if (i < size1) {
            best1 += nums1[i];
            ++i;
        } else if (j < size2) {
            best2 += nums2[j];
            ++j;
        }
    }

    return static_cast<int>(max(best1, best2) % MOD);
}

void processLine(const vector<int>& nums, ofstream& depthFile, ofstream& divisionFile, ofstream& sumFile) {
    int max_depth = recursiveDepth(nums, 0);
    depthFile << max_depth << endl;

    // Calculate division if possible and log
    if (nums.size() >= 2) {
        int dividend = abs(nums[0] - nums[1]);
        divisionFile << dividend << endl;
    }

    // Calculate sum and log
    int sum_result = 0;
    for (int num : nums) {
        sum_result += num;
    }
    sumFile << sum_result << endl;
}