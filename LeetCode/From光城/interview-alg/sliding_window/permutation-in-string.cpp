/*
 * @Author: gcxzhang
 * @Date: 2020-08-27 14:13:53
 * @LastEditTime: 2020-09-30 08:41:32
 * @Description: 字符串的排列
 * https://leetcode-cn.com/problems/permutation-in-string/
 * @FilePath: /alg/bin_search/permutation-in-string.cpp
 */

#include <string>
#include <unordered_map>
using namespace std;
class Solution {
 public:
  bool checkInclusion(string s1, string s2) {
    unordered_map<char, int> origin;
    for (auto& c : s1) {
      origin[c]++;
    }
    unordered_map<char, int> um;
    int l = 0, r = 0;
    int num = 0;
    while (r < s2.size()) {
      int cur = s2[r];
      r++;
      if (origin.count(cur)) {
        um[cur]++;
        if (origin[cur] == um[cur]) num++;
      }

      while (r - l >= s1.size()) {
        if (num == origin.size()) return true;
        int back = s2[l];
        if (origin.count(back)) {
          if (origin[back] == um[back]) num--;
          um[back]--;
        }
        l++;
      }
    }
    return false;
  }

  bool checkInclusion(string s1, string s2) {
    unordered_map<char, int> um;
    for (auto& c : s1) {
      um[c]++;
    }
    int l = 0, r = 0;
    while (r < s2.size()) {
      char c = s2[r++];
      um[c]--;
      while (l < r && um[c] < 0) {
        um[s2[l++]]++;
      }
      if (r - l == s1.size()) return true;
    }
    return false;
  }
};