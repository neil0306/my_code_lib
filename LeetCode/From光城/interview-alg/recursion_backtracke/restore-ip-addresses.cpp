/*
 * @Author: gcxzhang
 * @Date: 2020-08-27 21:25:27
 * @LastEditTime: 2020-09-30 08:40:57
 * @Description: 复原IP地址
 * https://leetcode-cn.com/problems/restore-ip-addresses/
 * @FilePath: /alg/recursion_backtracke/restore-ip-addresses.cpp
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;
class Solution {
 private:
  vector<string> ans;

 public:
  vector<string> restoreIpAddresses(string s) {
    dfs(s, 0, 0, "");
    return ans;
  }

  void dfs(string s, int depth, int index, string tmp) {
    if (depth > 4) return;
    if (depth == 4) {
      if (index == s.size()) {
        ans.push_back(tmp);
      }
      return;
    }
    for (int i = 1; i <= 3; i++) {
      if (index + i > s.size()) {
        return;
      }
      string cur = s.substr(index, i);
      if (stoi(cur) > 255 || (cur.size() > 1 && cur[0] == '0')) return;
      dfs(s, depth + 1, index + i, tmp + cur + (depth == 3 ? "" : "."));
    }
    return;
  }
};

int main() {
  Solution s;
  string ss = "25525511135";
  vector<string> vv = s.restoreIpAddresses(ss);
  for (auto& x : vv) {
    cout << x << endl;
  }
  return 0;
}