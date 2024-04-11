/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 09:59:46
 * @Description: 416. 分割等和子集
 * https://leetcode-cn.com/problems/partition-equal-subset-sum/
 * @FilePath: /alg/dp/背包/partition.cpp
 */

#include <iostream>
#include <vector>
using namespace std;

class Solution {
 public:
  bool canPartition(vector<int>& nums) {
    // 定义状态：dp[i][j]表示当容量/求和为j的时候，若只用前i个物品，存在/不存在子集的和可以恰好凑出sum/2
    int sum = 0;
    int n = nums.size();
    for (auto& item : nums) sum += item;
    if (sum & 1) return false;
    vector<vector<bool>> dp(n + 1, vector<bool>(sum / 2 + 1, false));
    for (int i = 0; i <= n; i++) {
      dp[i][0] = true;
    }
    for (int i = 1; i <= n; i++) {
      for (int j = 1; j <= sum / 2; j++) {
        if (j - nums[i - 1] < 0)
          dp[i][j] = dp[i - 1][j];
        else {
          dp[i][j] = dp[i - 1][j - nums[i - 1]] | dp[i - 1][j];
        }
      }
    }
    return dp[n][sum / 2];
  }
  // 状态压缩
  bool canPartition(vector<int>& nums) {
    // 定义状态：dp[i][j]表示当容量/求和为j的时候，若只用前i个物品，存在/不存在子集的和可以恰好凑出sum/2
    int sum = 0;
    int n = nums.size();
    for (auto& item : nums) sum += item;
    if (sum & 1) return false;
    vector<bool> dp(sum / 2 + 1, false);
    dp[0] = true;
    for (int i = 1; i <= n; i++) {
      for (int j = sum / 2; j >= 1; j--) {
        if (j - nums[i - 1] >= 0) dp[j] = dp[j - nums[i - 1]] | dp[j];
      }
    }
    return dp[sum / 2];
  }
};
