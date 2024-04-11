/*
 * @Author: gcxzhang
 * @Date: 2020-06-30 21:53:38
 * @LastEditTime: 2020-09-30 08:17:44
 * @Description:
 * @FilePath: /alg/slidingwindow/和为s的连续正整数序列.cpp
 */
#include <climits>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
using namespace std;
// 剑指 Offer 57 - II. 和为s的连续正数序列
class Solution {
 public:
  vector<vector<int>> findContinuousSequence(int target) {
    vector<vector<int>> res;
    int i = 1, j = 1;
    int sum = 0;
    while (i <= target / 2) {
      if (sum == target) {
        vector<int> tmp;
        for (int k = i; k < j; k++) {
          tmp.push_back(k);
        }
        res.push_back(tmp);
        sum -= i;
        i++;
      } else if (sum < target) {
        sum += j;
        j++;
      } else {
        sum -= i;
        i++;
      }
    }
    return res;
  }

  vector<vector<int>> findContinuousSequence1(int target) {
    vector<vector<int>> res;
    int i = 1, j = 2;
    int sum = 0;
    while (i <= j) {
      sum = (i + j) * (j - i + 1) / 2;
      if (sum == target) {
        vector<int> tmp;
        for (int k = i; k <= j; k++) {
          tmp.push_back(k);
        }
        res.push_back(tmp);
        i++;
      } else if (sum < target) {
        j++;
      } else {
        i++;
      }
    }
    return res;
  }
};

// 某一个大文件被拆成了N个小文件，每个小文件编号从0至N-1，相应大小分别记为S(i)。给定磁盘空间为C，试实现一个函数从N个文件中连续选出若干个文件拷贝到磁盘中，使得磁盘剩余空间最小。
vector<int> findMin(vector<int>& S, int C) {
  vector<int> ans;
  int l = 0, r = 0;
  int sum = 0;
  int minV = C;
  int i, j;
  while (l <= r) {
    if (sum <= C) {
      sum += S[r];
      r++;
      if (sum > C) {
        sum -= l;
        l++;
        continue;
      }
      if (minV > C - sum) {
        i = l;
        j = r;
      }
    } else {
      sum -= l;
      l++;
    }
  }
  for (int k = i; k < j; k++) {
    ans.push_back(S[k]);
  }
  return ans;
}

// LeetCode567. 字符串的排列
class Solution1 {
 public:
  bool checkInclusion(string s1, string s2) {
    if (s1.size() > s2.size()) return false;
    int window_size = s1.size();
    vector<int> m1(26, 0);
    vector<int> m2(26, 0);
    for (int i = 0; i < window_size; i++) {
      m1[s1[i] - 'a']++;
      m2[s2[i] - 'a']++;
    }
    for (int i = window_size; i < s2.size(); i++) {
      if (m1 == m2) return true;
      m2[s2[i - window_size] - 'a']--;
      m2[s2[i] - 'a']++;
    }
    return m1 == m2;
  }

  bool checkInclusion1(string s1, string s2) {
    unordered_map<char, int> um1, um2;
    for (char c : s1) um1[c]++;

    int left = 0, right = 0;
    int count = 0;
    while (right < s2.size()) {
      int c = s2[right];
      right++;
      if (um1.count(c)) {
        um2[c]++;
        if (um1[c] == um2[c]) count++;
      }
      while (right - left >= s1.size()) {
        // ！！！
        if (count == um1.size()) return true;
        char d = s2[left];
        left++;
        if (um1.count(d)) {
          if (um1[d] == um2[d]) count--;
          um2[d]--;
        }
      }
    }
    return false;
  }
};
/**
给定m个不重复的字符 [a, b, c,
d]，以及一个长度为n的字符串tbcacbdata，问能否在这个字符串中找到一个长度为m的连续子串，使得这个子串刚好由上面m个字符组成，顺序无所谓，返回任意满足条件的一个子串的起始位置，未找到返回-1。比如上面这个例子，acbd，3。
 */
int findStr(const vector<char>& ch, const string& s) {
  int window_size = ch.size();
  vector<int> v1(26, 0), v2(26, 0);
  for (int i = 0; i < window_size; i++) {
    v1[ch[i] - 'a']++;
    v2[s[i] - 'a']++;
  }
  for (int i = window_size; i < s.size(); i++) {
    if (v1 == v2) return i - window_size;
    v2[s[i - window_size] - 'a']--;
    v2[s[i] - 'a']++;
  }
  if (v1 == v2) {
    return s.size() - window_size;
  }
  return -1;
}

class Solution2 {
 public:
  // string minWindow(string s, string t) {

  // }
};

string minWindow(string s, string t) {
  unordered_map<char, int> um1, um2;
  for (char c : t) um1[c]++;

  int left = 0, right = 0;
  int count = 0;
  int start = 0;
  int len = INT_MAX;
  while (right < s.size()) {
    int c = s[right];
    right++;
    if (um1.count(c)) {
      um2[c]++;
      if (um1[c] == um2[c]) count++;
    }
    while (count == um1.size()) {
      // ！！！
      if (right - left < len) {
        len = right - left;
        start = left;
      }
      char d = s[left];
      left++;
      if (um1.count(d)) {
        if (um1[d] == um2[d]) count--;
        um2[d]--;
      }
    }
  }
  // 返回最小覆盖子串
  return len == INT_MAX ? "" : s.substr(start, len);
}

vector<int> findAnagrams(string s, string p) {
  unordered_map<char, int> um1, um2;
  for (auto& c : p) um1[c]++;
  vector<int> ans;
  int left = 0, right = 0;
  int count = 0;
  while (right < s.size()) {
    char c = s[right];
    right++;
    if (um1.count(c)) {
      um2[c]++;
      if (um1[c] == um2[c]) count++;
    }
    while (right - left >= p.size()) {
      if (count == um1.size()) {
        ans.push_back(left);
      }
      char d = s[left];
      left++;
      if (um1.count(d)) {
        if (um1[d] == um2[d]) count--;
        um2[d]--;
      }
    }
  }
  return ans;
}

int lengthOfLongestSubstring(string s) {
  unordered_map<char, int> um;

  int left = 0, right = 0;
  int ans = 0;
  while (right < s.size()) {
    char c = s[right];
    right++;
    um[c]++;
    while (um[c] > 1) {
      um[s[left]]--;
      left++;
    }
    ans = max(ans, right - left);
  }
  return ans;
}

int main() {
  string s = "pwwkew";
  cout << lengthOfLongestSubstring(s) << endl;

  string ss = "12,34,3,4,5";
  int index = ss.find(',');
  while (index != string::npos) {
    string m = ss.substr(0, index);
    cout << m << endl;
    ss = ss.substr(index + 1);
    index = ss.find(',');
  }
  cout << ss << endl;
}
