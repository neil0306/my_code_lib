/*
 * @Author: gcxzhang
 * @Date: 2020-06-30 10:30:24
 * @LastEditTime: 2020-09-30 10:05:15
 * @Description: 剑指 Offer 14- I. 剪绳子与剑指 Offer 14- II. 剪绳子 II
 * https://leetcode-cn.com/problems/jian-sheng-zi-lcof/
 * https://leetcode-cn.com/problems/jian-sheng-zi-ii-lcof/
 * @FilePath: /alg/dp/剪绳子/cuttingRope.cpp
 */
#include <iostream>
#include <vector>
using namespace std;
class Solution {
 public:
  int cuttingRope(int n) {
    if (n == 1 || n == 2) return 1;
    if (n == 3) return 2;
    int sum = 1;
    while (n > 4) {
      sum *= 3;
      n -= 3;
    }
    return sum * n;
  }

  int cuttingRope(int n) {
    // 1~n-1个物品 拼成重量为n的方案数/最大乘积
    vector<int> dp(n + 1, 1);
    dp[1] = 1;
    for (int i = 2; i <= n - 1; i++) {
      for (int j = 2; j <= n; j++) {
        if (j - i >= 0) {
          dp[j] = max(dp[j], dp[j - i] * i);
        } else {
          dp[j] = dp[j];
        }
      }
    }
    return dp[n];
  }
};