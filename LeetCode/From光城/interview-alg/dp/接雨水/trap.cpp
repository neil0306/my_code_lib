/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:50:53
 * @Description: 42.接雨水 https://leetcode-cn.com/problems/trapping-rain-water/
 * @FilePath: /alg/alg/dp/接雨水/trap.cpp
 */

#include <map>
#include <stack>
#include <vector>
using namespace std;
class Solution {
 public:
  int trap(vector<int>& height) {
    int n = height.size();
    // 拿栈顶的前两个与外部数据计算面积
    int area = 0;

    return area;
  }

  vector<int> singleStackTemp(vector<int>& nums) {
    int n = nums.size();
    vector<int> ans(n);
    stack<int> st;
    for (int i = n - 1; i >= 0; i--) {
      while (!st.empty() && st.top() <= nums[i]) {
        st.pop();
      }
      ans[i] = !st.empty() ? st.top() : -1;
      st.push(nums[i]);
    }
    return ans;
  }
  vector<int> dailyTemperatures(vector<int>& T) {
    int n = T.size();
    vector<int> ans(n);
    stack<int> st;
    for (int i = n - 1; i >= 0; i--) {
      while (!st.empty() && T[st.top()] <= T[i]) {
        st.pop();
      }
      ans[i] = !st.empty() ? st.top() - i : 0;
      st.push(i);
    }
  }

  // 环形单调栈
  vector<int> singleStackTempCircle(vector<int>& nums) {
    int n = nums.size();
    vector<int> ans(n);
    stack<int> st;
    for (int i = 2 * n - 1; i >= 0; i--) {
      while (!st.empty() && st.top() <= nums[i % n]) {
        st.pop();
      }
      ans[i % n] = !st.empty() ? st.top() : -1;
      st.push(nums[i % n]);
    }
    return ans;
  }
};

class Solution {
 public:
  vector<int> nextGreaterElement(vector<int>& nums1, vector<int>& nums2) {
    int n = nums2.size();
    vector<int> ans(nums1.size());
    stack<int> st;
    map<int, int> m;
    for (int i = n - 1; i >= 0; i--) {
      while (!st.empty() && st.top() <= nums2[i]) {
        st.pop();
      }
      int nextV = st.empty() ? -1 : st.top();
      m[nums2[i]] = nextV;
      st.push(nums2[i]);
    }

    for (int i = 0; i < nums1.size(); i++) {
      ans[i] = m[nums1[i]];
    }
    return ans;
  }
};

class Solution {
 public:
  // 单调递减栈
  int trap(vector<int>& height) {
    stack<int> st;
    int n = height.size();
    int ans = 0;
    for (int i = 0; i < n; i++) {
      while (!st.empty() && height[st.top()] < height[i]) {
        int cur = st.top();
        st.pop();
        if (st.empty()) break;
        int l = st.top();
        ans += (min(height[l], height[i]) - height[cur]) * (i - l - 1);
      }
      st.push(i);
    }
    return ans;
  }

  int trap(vector<int>& height) {
    stack<int> st;
    int n = height.size();
    int ans = 0;
    for (int i = n - 1; i >= 0; i--) {
      while (!st.empty() && height[st.top()] < height[i]) {
        int cur = st.top();
        st.pop();
        if (st.empty()) break;
        int r = st.top();
        ans += (min(height[r], height[i]) - height[cur]) * (r - i - 1);
      }
      st.push(i);
    }
    return ans;
  }

  int trap(vector<int>& height) {
    int n = height.size();
    int ans = 0;
    // dp[i] = min(dp[0...i], dp[i...n]) - height[i]
    for (int i = 1; i < n - 1; i++) {
      int l_max = 0, r_max = 0;
      for (int j = 0; j <= i; j++) {
        l_max = max(l_max, height[j]);
      }
      for (int j = i; j < n; j++) {
        r_max = max(r_max, height[j]);
      }
      ans += min(l_max, r_max) - height[i];
    }
    return ans;
  }

  int trap(vector<int>& height) {
    int n = height.size();
    if (n == 0) return 0;
    int ans = 0;
    // dp[i] = min(dp[0...i], dp[i...n]) - height[i]
    vector<int> l_max(n), r_max(n);
    l_max[0] = height[0];
    r_max[n - 1] = height[n - 1];
    for (int i = 1; i < n; i++) l_max[i] = max(l_max[i - 1], height[i]);

    for (int i = n - 2; i >= 0; i--) r_max[i] = max(r_max[i + 1], height[i]);

    for (int i = 1; i < n - 1; i++) ans += min(l_max[i], r_max[i]) - height[i];
    return ans;
  }

  int trap(vector<int>& height) {
    int n = height.size();
    if (n == 0) return 0;
    int r = n - 1;
    int l = 0;
    int l_max = height[0], r_max = height[n - 1];
    int ans = 0;
    while (l <= r) {
      l_max = max(l_max, height[l]);
      r_max = max(r_max, height[r]);

      if (l_max < r_max) {
        ans += l_max - height[l];
        l++;
      } else {
        ans += r_max - height[r];
        r--;
      }
    }
    return ans;
  }
};