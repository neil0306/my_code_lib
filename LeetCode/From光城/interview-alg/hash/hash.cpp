/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:16
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 11:29:22
 * @Description: 哈希表相关
 * @FilePath: /alg/alg/hash/hash.cpp
 */
#include <algorithm>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

// 128.最长连续序列
// https://leetcode-cn.com/problems/longest-consecutive-sequence/
class Solution {
 public:
  int longestConsecutive(vector<int>& nums) {
    // 是否是当前元素开头
    unordered_set<int> us(nums.begin(), nums.end());
    int ans = 0;
    for (auto& num : nums) {
      if (us.find(num - 1) == us.end()) {
        int offset = 1;
        while (us.find(num + offset) != us.end()) {
          offset += 1;
        }
        ans = max(ans, offset);
      }
    }
    return ans;
  }

 private:  // union find
  unordered_map<int, int> parent;
  int find(int p) {
    if (parent[p] == p) return p;
    return parent[p] = find(parent[p]);
  }

 public:
  int longestConsecutive1(vector<int>& nums) {
    for (auto elem : nums) parent[elem] = elem;

    for (auto elem : nums) {
      if (parent.count(elem - 1) > 0) parent[elem - 1] = elem;
      if (parent.count(elem + 1) > 0) parent[elem] = elem + 1;
    }
    int res = 0;
    for (auto elem : nums) {
      int x = find(elem);
      res = max(res, x - elem + 1);
    }
    return res;
  }
};

// 一个无序数组，从小到大找到第一个缺的数，比如[8 2 4 3 6 9 7 11
// 12],第一个缺的就是5

// 用数组作为哈希表，将数字i放入数组中的i索引处，然后找中间没有存入数字的位置。

int findLost(vector<int> nums) {
  int minV = nums[0];
  int maxV = nums[0];
  for (auto elem : nums) {
    minV = min(minV, elem);
    maxV = max(maxV, elem);
  }

  vector<int> res(maxV + 1, minV - 1);

  for (auto elem : nums) {
    res[elem] = elem;
  }
  for (int i = minV; i <= maxV; i++) {
    if (res[i] == minV - 1) {
      return i;
    }
  }
  return -1;
}

// 31.下一个排列
class Solution1 {
 public:
  void nextPermutation(vector<int>& nums) {
    int n = nums.size();
    if (n <= 1) return;
    int i = n - 2;
    for (; i >= 0 && nums[i] >= nums[i + 1]; i--) {
    }
    if (i == -1) {
      sort(nums.begin(), nums.end());
      return;
    }
    int j = n - 1;
    for (; j > i && nums[i] >= nums[j]; j--) {
    }
    swap(nums[i], nums[j]);
    reverse(nums.begin() + i + 1, nums.end());
  }
};
// 55.跳跃游戏
class Solution3 {
 public:
  bool canJump(vector<int>& nums) {
    int maxV = 0;
    for (int i = 0; i < nums.size(); i++) {
      if (maxV < i) return false;
      maxV = max(maxV, nums[i] + i);
    }

    return true;
  }
};
// AB两个排序数组，原地合并数组。（A当中穿插一些无效数字怎么处理？）
void merge(vector<int>& A, int m, vector<int>& B, int n) {
  int k = m + n - 1;
  int a_j = m - 1;
  int b_j = n - 1;
  while (a_j >= 0 || b_j >= 0) {
    if (a_j < 0) {
      while (b_j >= 0) {
        A[k--] = B[b_j--];
      }
      break;
    } else if (b_j < 0) {
      while (a_j >= 0) {
        A[k--] = A[a_j--];
      }
      break;
    } else {
      if (A[a_j] > B[b_j]) {
        A[k--] = A[a_j--];
      } else {
        A[k--] = B[b_j--];
      }
    }
  }
}
int main() {
  vector<int> nums{8, 2, 4, 3, 6, 9, 7, 11, 12};

  cout << findLost(nums) << endl;
  nums = {1, 2, 8, 3, 1};
  Solution1().nextPermutation(nums);
  for (auto elem : nums) {
    cout << elem << " ";
  }
  cout << endl;

  vector<int> v1 = {1, 5, 7, 9, 10};
  v1.resize(10);
  vector<int> v2 = {2, 4, 8, 12, 20};
  v2.resize(5);
  merge(v1, 5, v2, 5);
  for (auto elem : v1) {
    cout << elem << " ";
  }
  cout << endl;
}
