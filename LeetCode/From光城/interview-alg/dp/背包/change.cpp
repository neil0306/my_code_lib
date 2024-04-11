/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:43:50
 * @Description: 518.零钱兑换 II
 * https://leetcode-cn.com/problems/coin-change-2/
 * @FilePath: /alg/alg/dp/背包/change.cpp
 */
#include <iostream>
#include <vector>
using namespace std;

class Solution {
 public:
  int change(int amount, vector<int>& coins) {
    int n = coins.size();
    vector<vector<int>> dp(
        n + 1, vector<int>(amount + 1,
                           0));  // 定义状态 dp[i][j]表示前i个硬币凑成j的方案数

    // base case
    for (int i = 0; i <= n; i++) {
      dp[i][0] = 1;
    }
    for (int i = 1; i <= n; i++) {
      for (int j = 1; j <= amount; j++) {
        if (j - coins[i - 1] < 0)
          dp[i][j] = dp[i - 1][j];
        else
          // 一种是没有用前i-1个硬币就凑齐了
          // 一种是前面已经凑到了j-i，现在就差第i个硬币了。
          dp[i][j] = dp[i][j - coins[i - 1]] + dp[i - 1][j];
      }
    }

    return dp[n][amount];
  }
};

class Solution {
 public:
  int change(int amount, vector<int>& coins) {
    int n = coins.size();
    // 状态数组dp[i]表示的是对于第i个硬币能凑的组合数/方案数 必须使用第i个硬币
    vector<int> dp(amount + 1, 0);
    // base case
    dp[0] = 1;
    for (int i = 1; i <= n; i++) {
      for (int j = 1; j <= amount; j++) {
        if (j - coins[i - 1] >= 0)
          // dp[j] = dp[j - coins[i - 1]] + dp[j];
          dp[j] += dp[j - coins[i - 1]];
      }
    }
    return dp[amount];
  }
};

int main() { return 0; }
