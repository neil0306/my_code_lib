/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:53:37
 * @Description: 72. 编辑距离 https://leetcode-cn.com/problems/edit-distance/
 * @FilePath: /alg/alg/dp/最大编辑距离/minDistance.cpp
 */

#include <iostream>
#include <vector>
using namespace std;

enum OP { OP_NO_CHANGE = 0, OP_REPLACE = 1, OP_INSERT = 2, OP_DELETE = 3 };
struct Node {
  int op{OP_NO_CHANGE};  // 操作
  int val;               // 距离
};
class Solution {
 public:
  int minDistance(string word1, string word2) {
    int n = word1.size();
    int m = word2.size();

    vector<vector<int>> dp(n + 1, vector<int>(m + 1));
    // base case
    for (int i = 0; i <= n; i++) {
      dp[i][0] = i;
    }
    for (int j = 0; j <= m; j++) {
      dp[0][j] = j;
    }

    for (int i = 1; i <= n; i++) {
      for (int j = 1; j <= m; j++) {
        if (word1[i - 1] == word2[j - 1])
          dp[i][j] = dp[i - 1][j - 1];
        else
          dp[i][j] = 1 + min(min(dp[i - 1][j], dp[i][j - 1]), dp[i - 1][j - 1]);
      }
    }
    int n_r = n, m_r = m;
    // 回溯得到 路径
    while (n >= 0 || m >= 0) {
      if (n && dp[n][m] - 1 == dp[n - 1][m]) {
        cout << n << "," << m << "删除" << endl;
        n -= 1;
        continue;
      }
      if (m && dp[n][m] - 1 == dp[n][m - 1]) {
        cout << n << "," << m << "插入" << endl;
        m -= 1;
        continue;
      }
      if (n && m && dp[n][m] - 1 == dp[n - 1][m - 1]) {
        cout << n << "," << m << "替换" << endl;
        n -= 1;
        m -= 1;
        continue;
      }
      if (n && m && dp[n][m] == dp[n - 1][m - 1]) {
        cout << n << "," << m << "不改变" << endl;
      }
      n -= 1;
      m -= 1;
    }
    return dp[n_r][m_r];
  }
};
int main() {
  Solution s;
  cout << s.minDistance("intention", "execution") << endl;

  return 0;
}
