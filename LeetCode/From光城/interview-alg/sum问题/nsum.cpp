/*
 * @Author: gcxzhang
 * @Date: 2020-07-07 12:30:30
 * @LastEditTime: 2020-09-30 08:27:19
 * @Description: leetcode 2数之和、3数之和、nSum问题 、大数之和
 * @FilePath: /alg/sort/nsum.cpp
 */
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

// nsum
vector<vector<int>> nSumRecursive(vector<int> nums, int target, int x,
                                  int start) {
  int n = nums.size();
  int l = start;
  int r = n - 1;
  vector<vector<int>> res;
  if (x < 2 || n < x) return res;
  if (x == 2) {
    while (l < r) {
      int left = nums[l];
      int right = nums[r];
      if (nums[l] + nums[r] == target) {
        while (l < r && left == nums[l]) {
          l++;
        }
        while (l < r && right == nums[r]) {
          r--;
        }
        vector<int> tmp{left, right};
        res.push_back(tmp);
      } else if (nums[l] + nums[r] > target) {
        while (l < r && right == nums[r]) {
          r--;
        }
      } else {
        while (l < r && left == nums[l]) {
          l++;
        }
      }
    }
  } else {
    for (int i = start; i < nums.size() - 1; i++) {
      vector<vector<int>> sub =
          nSumRecursive(nums, target - nums[i], x - 1, i + 1);
      for (auto& item_vec : sub) {
        item_vec.push_back(nums[i]);
        res.push_back(item_vec);
      }
      while (i < nums.size() - 2 && nums[i] == nums[i + 1]) {
        i++;
      }
    }
  }
  return res;
}
vector<vector<int>> nSum(vector<int> nums, int target) {
  sort(nums.begin(), nums.end());
  return nSumRecursive(nums, target, 3, 0);
}

// 大数之和
class Solution {
 public:
  string addStrings(string num1, string num2) {
    int n = num1.size();
    int m = num2.size();
    if (n == 0 || m == 0 ||
        n == 1 && num1[0] == '0' && m == 1 && num2[0] == '0')
      return "0";

    if (n < m) {
      swap(num1, num2);
      swap(n, m);
    }

    string res(n + m, '0');
    int len = res.size() - 1;
    int carry = 0;
    int i = n - 1, j = m - 1;
    while (i >= 0) {
      int cur = num1[i] - '0';
      if (j >= 0) {
        cur += (num2[j] - '0');
      }
      cur += carry;
      res[len] = cur % 10 + '0';
      carry = cur / 10;
      len--;
      i--;
      j--;
    }
    if (carry) {
      res[len] = carry + '0';
    }
    int k = 0;
    while (res[k] == '0') {
      k++;
    }
    return res.substr(k);
  }
};
int main() {
  vector<int> nums{5, 1, 0, 0, 10, 2, 3};
  int target = 5;
  vector<vector<int>> res = nSum(nums, target);
  for (auto v : res) {
    for (auto item : v) {
      cout << item << " ";
    }
    cout << endl;
  }

  cout << Solution().addStrings("1", "9") << endl;
  return 0;
}