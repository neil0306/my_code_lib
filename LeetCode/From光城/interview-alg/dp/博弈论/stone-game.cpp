/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:00:58
 * @Description: 877. 石子游戏 https://leetcode-cn.com/problems/stone-game/
 * @FilePath: /alg/dp/博弈论/stone-game.cpp
 */
#include <iostream>
#include <vector>
using namespace std;
// 同最长回文子序列解法
class Solution {
 public:
  bool stoneGame(vector<int> &piles) {
    int n = piles.size();
    // dp[i][j].first表示，对于 piles[i...j]
    // 这部分石头堆，先手能获得的最高分数。 dp[i][j].second表示，对于
    // piles[i...j] 这部分石头堆，后手能获得的最高分数。

    vector<vector<pair<int, int>>> dp(n, vector<pair<int, int>>(n));
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        dp[i][j] = make_pair(0, 0);
      }
    }
    // base case 只有一堆
    for (int i = 0; i < n; i++) {
      dp[i][i].first = piles[i];
      dp[i][i].second = 0;
    }
    // 从下往上 从左到右
    for (int i = n - 1; i >= 0; i--) {
      for (int j = i + 1; j < n; j++) {
        // 要么我选择最左边的那一堆石头，然后面对
        // piles[i+1...j],但是此时轮到对方，相当于我变成了后手。
        int left = piles[i] + dp[i + 1][j].second;
        // 要么我选择最右边的那一堆石头，然后面对
        // piles[i...j-1],但是此时轮到对方，相当于我变成了后手。
        int right = piles[j] + dp[i][j - 1].second;

        if (left > right) {
          dp[i][j].first = left;
          // 如果先手选择了最左边那堆，给我剩下了 piles[i+1...j]
          dp[i][j].second = dp[i + 1][j].first;
        } else {
          dp[i][j].first = right;
          // 如果先手选择了最右边那堆，给我剩下了 piles[i...j-1]
          dp[i][j].second = dp[i][j - 1].first;
        }
      }
    }
    return dp[0][n - 1].first > dp[0][n - 1].second ? true : false;
  }

  bool stoneGame(vector<int> &piles) {
    int n = piles.size();
    vector<vector<pair<int, int>>> dp(n, vector<pair<int, int>>(n));
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        dp[i][j] = make_pair(0, 0);
      }
    }
    // base case
    for (int i = 0; i < n; i++) {
      dp[i][i].first = piles[i];
      dp[i][i].second = 0;
    }
    // 斜着遍历
    for (int step = 1; step < n; step++) {
      for (int i = 0; i < n - step; i++) {
        int j = i + step;
        int left = piles[i] + dp[i + 1][j].second;
        int right = piles[j] + dp[i][j - 1].second;

        if (left > right) {
          dp[i][j].first = left;
          dp[i][j].second = dp[i + 1][j].first;
        } else {
          dp[i][j].first = right;
          dp[i][j].second = dp[i][j - 1].first;
        }
      }
    }
    return dp[0][n - 1].first > dp[0][n - 1].second ? true : false;
  }
};
