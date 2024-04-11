/*
 * @Author: gcxzhang
 * @Date: 2020-08-27 13:56:13
 * @LastEditTime: 2020-09-30 10:41:30
 * @Description: 3.无重复字符的最长子串
 * https://leetcode-cn.com/problems/longest-substring-without-repeating-characters/
 * @FilePath:
 * /alg/alg/bin_search/longest-substring-without-repeating-characters.cpp
 */
#include <unordered_map>
using namespace std;

class Solution {
 public:
  int lengthOfLongestSubstring(string s) {
    unordered_map<char, int> um;

    int ans = 0;

    int l = 0, r = 0;
    while (r < s.size()) {
      um[s[r]]++;
      while (um[s[r]] > 1) {
        um[s[l]]--;
        l++;
      }
      r++;
      ans = max(ans, r - l);
    }
    return ans;
  }
};