/*
 * @Author: gcxzhang
 * @Date: 2020-07-05 15:29:27
 * @LastEditTime: 2020-09-30 08:17:54
 * @Description:
 * @FilePath: /alg/sort/basic_sort.cpp
 */
#include <iostream>
#include <vector>
using namespace std;
// 插入排序
void InsertSort(vector<int>& nums) {
  int n = nums.size();
  for (int i = 1; i < n; i++) {
    int cur = nums[i];
    int j;
    for (j = i; j > 0 && cur < nums[j - 1]; j--) {
      nums[j] = nums[j - 1];
    }
    nums[j] = cur;
  }
}

// 冒泡排序
void BubbleSort(vector<int>& nums) {
  int n = nums.size();
  for (int i = n - 1; i >= 0; i--) {
    for (int j = 0; j < i; j++) {
      if (nums[j] > nums[j + 1]) swap(nums[j], nums[j + 1]);
    }
  }
}
int partition(vector<int>& nums, int l, int mid, int r) {
  swap(nums[l], nums[rand() % (r - l + 1) + l]);
  int v = nums[l];
  int j = l;  // [l+1...j]
  for (int i = l + 1; i <= r; i++) {
    if (nums[i] < v) {
      swap(nums[i], nums[j + 1]);
      j++;
    }
  }
  swap(nums[l], nums[j]);
  return j;
}
int partition2(vector<int>& nums, int l, int mid, int r) {
  swap(nums[l], nums[rand() % (r - l + 1) + l]);
  int v = nums[l];
  int i = l + 1, j = r;
  while (i <= j) {
    while (i <= j && nums[i] < v) {
      i++;
    }
    while (i <= j && nums[j] > v) {
      j--;
    }
    if (i > j) break;
    swap(nums[i], nums[j]);
    i++;
    j--;
  }
  swap(nums[l], nums[j]);
  return j;
}
void quickSort(vector<int>& nums, int l, int r) {
  if (l >= r) return;
  int mid = l + (r - l) / 2;
  int p = partition2(nums, l, mid, r);
  quickSort(nums, l, p - 1);
  quickSort(nums, p + 1, r);
}

void quickSortThreeWays(vector<int>& nums, int l, int r) {
  if (l >= r) return;

  swap(nums[l], nums[rand() % (r - l + 1) + l]);
  int v = nums[l];
  // [l+1...lt] [lt+1...i) [gt...r]
  int lt = l;
  int i = l + 1;
  int gt = r + 1;
  while (i < gt) {
    if (nums[i] == v) {
      i++;
    } else if (nums[i] > v) {
      swap(nums[i], nums[gt - 1]);
      gt--;
    } else {
      swap(nums[i], nums[lt + 1]);
      lt++;
      i++;
    }
  }
  swap(nums[l], nums[lt]);
  quickSortThreeWays(nums, l, lt - 1);
  quickSortThreeWays(nums, gt, r);
}

void QuickSort(vector<int>& nums) {
  quickSortThreeWays(nums, 0, nums.size() - 1);
}

void merge(vector<int>& nums, int l, int mid, int r) {
  vector<int> newNums(r - l + 1);
  for (int i = l; i <= r; i++) {
    newNums[i - l] = nums[i];
  }

  for (int k = l, i = l, j = mid + 1; k <= r; k++) {
    if (i > mid) {
      nums[k] = newNums[j - l];
      j++;
    } else if (j > r) {
      nums[k] = newNums[i - l];
      i++;
    } else if (newNums[i - l] < newNums[j - l]) {
      nums[k] = newNums[i - l];
      i++;
    } else {
      nums[k] = newNums[j - l];
      j++;
    }
  }
}

void mergeSort(vector<int>& nums, int l, int r) {
  if (l >= r) return;
  int mid = l + (r - l) / 2;
  mergeSort(nums, l, mid);
  mergeSort(nums, mid + 1, r);
  merge(nums, l, mid, r);
}

// 归并排序
void MergeSort(vector<int>& nums) { mergeSort(nums, 0, nums.size() - 1); }

void MergeSort1(vector<int>& nums) {
  int n = nums.size();
  for (int sz = 1; sz < n; sz += sz) {
    for (int j = 0; j + sz < n; j++) {
      merge(nums, j, j + sz - 1, min(j + sz + sz - 1, n - 1));
    }
  }
}

int main() {
  vector<int> nums{1, 10, 0, -1, 8};
  MergeSort(nums);
  for (auto& num : nums) {
    cout << num << " ";
  }
  cout << endl;
  return 0;
}