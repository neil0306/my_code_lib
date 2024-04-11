/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:55:50
 * @Description:
 * @FilePath: /alg/alg/dp/rob/rob.cpp
 */
#include <iostream>
#include <vector>
using namespace std;
class Solution {
 public:
  int rob(vector<int>& nums) {
    if (nums.size() == 0) return 0;
    if (nums.size() == 1) return nums[0];
    vector<int> nums0 = nums;
    vector<int> nums1 = nums;

    nums1.erase(nums1.begin());
    return max(_rob(nums0), _rob(nums1));
  }

  int _rob(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    vector<int> dp(n + 1, 0);
    dp[0] = 0;
    dp[1] = nums[0];
    for (int i = 2; i <= n; i++) {
      dp[i] = max(dp[i - 2] + nums[i - 1], dp[i - 1]);
    }
    return dp[n];
  }
};

int main() {
  vector<int> v{1, 2, 1, 1};
  cout << Solution().rob(v);

  return 0;
}