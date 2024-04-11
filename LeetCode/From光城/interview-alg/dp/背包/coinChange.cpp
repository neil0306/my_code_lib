/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 09:52:57
 * @Description: 322. 零钱兑换 https://leetcode-cn.com/problems/coin-change/
 * @FilePath: /alg/dp/背包/coinChange.cpp
 */

#include <iostream>
#include <queue>
#include <set>
#include <vector>
using namespace std;
class Solution {
 public:
  int coinChange(vector<int> &coins, int amount) {
    queue<pair<int, int>> q;
    set<int> visited;
    q.push(make_pair(amount, 0));  // 剩余的钱与已用硬币个数

    while (!q.empty()) {
      int red = q.front().first;
      int num = q.front().second;
      for (auto co : coins) {
        if (co == red)
          return num + 1;
        else if (red > co && !visited.count(co)) {
          q.push(make_pair(amount - co, num + 1));
          visited.insert(co);
        }
      }
    }
    return -1;
  }
};

int main() { return 0; }
