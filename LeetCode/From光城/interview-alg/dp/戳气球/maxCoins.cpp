/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:02:42
 * @Description: 312. 戳气球 https://leetcode-cn.com/problems/burst-balloons/
 * @FilePath: /alg/dp/戳气球/maxCoins.cpp
 */
#include <vector>
using namespace std;
class Solution {
 public:
  int maxCoins(vector<int> &nums) {
    int n = nums.size();
    nums.insert(nums.begin(), 1);
    nums.insert(nums.begin() + nums.size(), 1);

    // dp[i][j] i~j区间 两边开戳最后一个气球k,所获得的最大硬币数量
    vector<vector<int>> dp(n + 2, vector<int>(n + 2, 0));

    for (int step = 1; step < n + 2; step++) {
      for (int i = 0; i < n + 2 - step; i++) {
        int j = i + step;
        for (int k = i + 1; k < j; k++) {
          dp[i][j] =
              max(dp[i][j], dp[i][k] + dp[k][j] + nums[i] * nums[k] * nums[j]);
        }
      }
    }
    return dp[0][n + 1];
  }
};