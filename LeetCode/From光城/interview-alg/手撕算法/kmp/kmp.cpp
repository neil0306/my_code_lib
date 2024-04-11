/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:53:54
 * @Description: kmp
 * @FilePath: /alg/alg/dp/kmp/kmp.cpp
 */

#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

// pat M  txt N
// 暴力法
int Match(string pat, string txt) {
  int m = pat.size();
  int n = txt.size();
  // txt串走到最后一个匹配起始位置
  for (int i = 0; i <= n - m; i++) {
    int j;
    for (j = 0; j < m; j++) {
      if (pat[j] != txt[i + j]) break;
    }
    if (j == m)  // 匹配
      return i;
  }
  return -1;  // 不匹配
}

// 二维dp的kmp
// dp状态定义：dp[i][c] = next
// 表示模式串中第i个状态匹配下一字符c应该转移到哪个状态
int Match_dp(string pat, string txt) {
  int m = pat.size();
  int n = txt.size();
  vector<vector<int>> dp(m, vector<int>(256, 0));
  // 构建dp数组
  int prev_status = 0;  // 前一状态
  // base case
  dp[0][pat[0]] =
      1;  // 遇到pat[0]这个字符才能更新状态从0到1，遇到其他字符还是停留在状态0
  for (int i = 1; i < m; i++) {
    for (int c = 0; c < 256; c++) {
      if (pat[i] == c) {
        dp[i][c] = i + 1;  // 匹配上前进
      } else {
        dp[i][c] = dp[prev_status][c];  // 使用前一状态更新
      }
    }
    // 当前状态prev_status遇到pat[i]转向哪
    prev_status = dp[prev_status][pat[i]];
  }

  // 搜索
  int i = 0;                     // pat状态
  for (int j = 0; j < n; j++) {  // 不断遍历原串
    i = dp[i][txt[j]];           // 下一状态
    if (i == m) return j - m + 1;
  }
  return -1;
}
int main() {
  cout << Match("abab", "abacabab") << endl;
  cout << Match_dp("abab", "abacabab") << endl;
  return 0;
}