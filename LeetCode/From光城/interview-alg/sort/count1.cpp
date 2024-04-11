/*
 * @Author: 光城
 * @Date: 2020-09-30 08:33:21
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 08:33:24
 * @FilePath: /alg/sort/count1.cpp
 */
// 233. 数字 1 的个数 https://leetcode-cn.com/problems/number-of-digit-one/
// 拓展位 数字1~n中x出现的次数 时间复杂度O(logn)  以10为底

#include <iostream>
using namespace std;
class Solution {
 public:
  int countDigitOne(int n, int x) {
    if (n < 1) {
      return 0;
    }
    long long base = 1;
    int pre = n;
    int count = 0;

    while (pre > 0) {
      int cur = pre % 10;
      pre = pre / 10;
      if (cur == 0) {
        count += pre * base;
      } else if (cur == x) {
        count += pre * base + n % base + 1;
      } else if (cur > x) {
        count += pre * base + base;
      }
      base *= 10;
    }
    return count;
  }
};
int main() {
  int n = 260;
  int m = 5;
  cout << Solution().countDigitOne(n, m);
  return 0;
}