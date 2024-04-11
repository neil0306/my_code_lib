/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 09:52:49
 * @Description: 70. 爬楼梯 https://leetcode-cn.com/problems/climbing-stairs/
 * @FilePath: /alg/dp/背包/climbStairs.cpp
 */

#include <vector>
using namespace std;
class Solution {
 public:
  int climbStairs(int n) {
    vector<int> dp(n + 1, 0);
    dp[0] = 1;
    dp[1] = 1;
    for (int i = 2; i <= n; i++) {
      for (int j = 1; j <= 2; j++) {
        dp[i] += dp[i - j];
      }
    }
    return dp[n];
  }
};
