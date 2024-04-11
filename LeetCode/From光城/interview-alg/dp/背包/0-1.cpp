/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 09:45:29
 * @Description: 0-1背包
 * @FilePath: /alg/dp/0-1背包/0-1.cpp
 */
#include <iostream>
#include <vector>
using namespace std;

// 0-1背包
int main() {
  int n = 3;  // 物品个数
  int w = 4;  // 背包承载的总重量
  int weight[] = {2, 1, 3};
  int value[] = {4, 2, 3};

  // 定义状态：dp[i][j] 表示第i个物品在容量为j的时候所获得的最大价值
  vector<vector<int>> dp(n + 1, vector<int>(w + 1, 0));

  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= w; j++) {
      if (j - weight[i - 1] < 0) {
        dp[i][j] = dp[i - 1][j];
      } else {
        dp[i][j] =
            max(dp[i - 1][j - weight[i - 1]] + value[i - 1], dp[i - 1][j]);
      }
    }
  }

  cout << "最大价值:" << dp[n][w] << endl;

  vector<int> dp1(w + 1, 0);
  for (int i = 1; i <= n; i++)
    for (int j = w; j >= 1; j--)
      if (j - weight[i - 1] >= 0)
        dp1[j] = max(dp1[j - weight[i - 1]] + value[i - 1], dp1[j]);
  cout << "最大价值:" << dp1[w] << endl;
  return 0;
}
