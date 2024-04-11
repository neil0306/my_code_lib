/*
 * @Author: gcxzhang
 * @Date: 2020-06-30 21:09:11
 * @LastEditTime: 2020-09-30 11:14:58
 * @Description:
 * @FilePath: /alg/alg/二分/bin.cpp
 */
// 排序数组，平方后，数组当中有多少不同的数字（相同算一个）。
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
int diffSquareNum(const vector<int>& nums) {
  int n = nums.size();

  int l = 0, r = n - 1;
  int ans = 0;
  while (l <= r) {
    if (nums[l] + nums[r] == 0) {
      ans += 1;
      int temp = nums[l];
      while (l <= r && nums[l] == temp) {
        l++;
      }
      while (l <= r && nums[r] == -temp) {
        r--;
      }
    } else if (nums[l] + nums[r] > 0) {
      ans += 1;
      int temp = nums[r];
      while (l <= r && nums[r] == temp) {
        r--;
      }
    } else {
      ans += 1;
      int temp = nums[l];
      while (l <= r && nums[l] == temp) {
        l++;
      }
    }
  }
  return ans;
}

//一个数据先递增再递减，找出数组不重复的个数，比如 [1, 3, 9,
// 1]，结果为3，不能使用额外空间，复杂度o(n)

int diffNum(const vector<int>& nums) {
  int n = nums.size();
  int ans = 0;

  int l = 0, r = n - 1;

  while (l <= r) {
    if (nums[l] == nums[r]) {
      ans += 1;
      int temp = nums[l];
      while (l <= r && nums[l] == temp) {
        l++;
      }
      while (l <= r && nums[r] == temp) {
        r--;
      }
    } else if (nums[l] > nums[r]) {
      ans += 1;
      int temp = nums[r];
      while (l <= r && nums[r] == temp) {
        r--;
      }
    } else {
      ans += 1;
      int temp = nums[l];
      while (l <= r && nums[l] == temp) {
        l++;
      }
    }
  }
  return ans;
}

// 递增数组，找出和为k的数对
vector<int> findPair(vector<int> nums, int k) {
  vector<int> res;
  int n = nums.size();
  if (n < 2) return res;
  int l = 0, r = n - 1;
  while (l < r) {
    if (nums[l] + nums[r] == k) {
      res.push_back(nums[l]);
      res.push_back(nums[r]);
    } else if (nums[l] + nums[r] > k) {
      r--;
    } else {
      l++;
    }
  }
  return res;
}

// 给出一个数组nums，一个值k，找出数组中的两个下标 i，j 使得 nums[i] + nums[j] =
// k.
vector<int> twoSum(vector<int>& nums, int target) {
  int n = nums.size();
  vector<int> res;
  map<int, int> m;
  for (int i = 0; i < n; i++) {
    if (m.count(target - nums[i])) {
      res.push_back(i);
      res.push_back(m[target - nums[i]]);
    } else
      m[nums[i]] = i;
  }
  return res;
}

vector<int> twoSum(vector<int>& nums, int target) {
  vector<int> v = nums;
  sort(v.begin(), v.end());
  vector<int> res;
  int l = 0, r = v.size() - 1;
  while (l < r) {
    if (v[l] + v[r] == target) {
      for (int k = 0; k < nums.size(); k++) {
        if (nums[k] == v[l] || nums[k] == v[r]) {
          res.push_back(k);
        }
      }
      break;
    } else if (v[l] + v[r] > target) {
      r--;
    } else {
      l++;
    }
  }
  return res;
}
