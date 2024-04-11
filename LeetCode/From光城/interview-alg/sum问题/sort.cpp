/*
 * @Author: gcxzhang
 * @Date: 2020-07-04 15:09:59
 * @LastEditTime: 2020-07-04 18:10:08
 * @Description:
 * @FilePath: /alg/排序/sort.cpp
 */
#include <iostream>
#include <vector>
#include <climits>
using namespace std;
// 315. 计算右侧小于当前元素的个数
class Solution {
private:
    vector<int> ans;
    vector<pair<int, int>> elements;
public:
    vector<int> countSmaller(vector<int>& nums) {
        elements = vector<pair<int, int>>();
        int n = nums.size();
        for(int i = 0; i < n; i ++)
            elements.push_back(make_pair(nums[i], i));
        ans = vector<int>(n);
        mergeSort(elements,0,n-1);
        return ans;
    }
    void mergeSort(vector<pair<int,int>>& nums,int l,int r) {
        if(l>=r) return;
        int mid = l+(r-l)/2;
        mergeSort(nums,l,mid);
        mergeSort(nums,mid+1,r);
        merge(nums,l,mid,r);

    }
    long long merge(vector<pair<int,int>>& nums,int l,int mid, int r) {
        vector<pair<int,int>> new_nums(r-l+1);
        for (int i=l;i<=r;i++) {
            new_nums[i-l] = nums[i];
        }
        long long res = 0;

        for (int k=l,i=l,j=mid+1;k<=r;k++) {
            if(i>mid) {
                nums[k] = new_nums[j-l];
                j++;
            } else if(j>r) {
                nums[k] = new_nums[i-l];
                ans[new_nums[i-l].second] += j-mid-1;
                i++;
            } else if(new_nums[i-l] <= new_nums[j-l]) {
                nums[k] = new_nums[i-l];
                ans[new_nums[i-l].second] += j-mid-1;
                i++;
            } else {
                nums[k] = new_nums[j-l];
                res += mid-i+1;

                j++;
            }
        }
        return res;
    }
};


// 剑指 Offer 51. 数组中的逆序对
class Solution1 {
public:
    int reversePairs(vector<int>& nums) {
        return mergeSort(nums,0,nums.size()-1);
    }
    long long mergeSort(vector<int>& nums,int l,int r) {
        if(l>=r) return 0;
        int mid = l+(r-l)/2;
        long long left_res = mergeSort(nums,l,mid);
        long long right_res = mergeSort(nums,mid+1,r);
        if(nums[mid] > nums[mid + 1])
            return left_res+right_res+merge(nums,l,mid,r);
        return left_res+right_res;
    }
    long long merge(vector<int>& nums,int l,int mid, int r) {
        vector<int> new_nums(r-l+1);
        for (int i=l;i<=r;i++) {
            new_nums[i-l] = nums[i];
        }
        long long res = 0;

        for (int k=l,i=l,j=mid+1;k<=r;k++) {
            if(i>mid) {
                nums[k] = new_nums[j-l];
                j++;
            } else if(j>r) {
                nums[k] = new_nums[i-l];
                i++;
            } else if(new_nums[i-l] <= new_nums[j-l]) {
                nums[k] = new_nums[i-l];
                i++;
            } else {
                nums[k] = new_nums[j-l];
                res += mid-i+1;
                j++;
            }
        }
        return res;
    }
};


// 215. 数组中的第K个最大元素
class Solution2 {
public:
    int findKthLargest(vector<int>& nums, int k) {
        int n = nums.size();
        // srand(time(NULL));
        return __quickSort(nums,0,n-1,n-k);
    }
    int __quickSort(vector<int>& nums,int l,int r,int target) {
        if (l>=r) return -1;
        // partition之后, arr[p]的正确位置就在索引p上
        int p = partition( nums, l, r );

        if( target == p )    // 如果 k == p, 直接返回arr[p]
            return nums[p];
        else if( target < p )    // 如果 k < p, 只需要在arr[l...p-1]中找第k小元素即可
            return __quickSort( nums, l, p-1, target);
        else // 如果 k > p, 则需要在arr[p+1...r]中找第k-p-1小元素
            // 注意: 由于我们传入__selection的依然是arr, 而不是arr[p+1...r],
            //       所以传入的最后一个参数依然是k, 而不是k-p-1
            return __quickSort( nums, p+1, r, target );
    }

    int partition(vector<int>&nums, int l, int r) {
        swap(nums[l],nums[rand()%(r-l+1)+l]);
        int privot = nums[l];
        int j = l;
        for (int i=l+1;i<=r;i++) {
            if (nums[i]<privot) {
                swap(nums[j+1],nums[i]);
                j++;
            }
        }
        swap(nums[l],nums[j]);
        return j;
    }
};

class Solution3 {
private:
  vector<int> ans;
public:
    vector<int> findKthLargest(vector<int>& nums, int k) {
        int n = nums.size();
        ans = vector<int>();
        // srand(time(NULL));
        __quickSort(nums,0,n-1,n-k);
        return ans;
    }
    void __quickSort(vector<int>& nums,int l,int r,int target) {
        if (l>=r) return;
        if (target == nums.size()) return;
        // partition之后, arr[p]的正确位置就在索引p上
        int p = partition( nums, l, r );

        if( target == p ) {   // 如果 k == p, 直接返回arr[p]
          ans.push_back(nums[target]);
          __quickSort( nums, 0, nums.size()-1, target+1);
        } else if( target < p )    // 如果 k < p, 只需要在arr[l...p-1]中找第k小元素即可
            __quickSort( nums, l, p-1, target);
        else // 如果 k > p, 则需要在arr[p+1...r]中找第k-p-1小元素
            // 注意: 由于我们传入__selection的依然是arr, 而不是arr[p+1...r],
            //       所以传入的最后一个参数依然是k, 而不是k-p-1
            __quickSort( nums, p+1, r, target );
    }

    int partition(vector<int>&nums, int l, int r) {
        swap(nums[l],nums[rand()%(r-l+1)+l]);
        int privot = nums[l];
        int j = l;
        for (int i=l+1;i<=r;i++) {
            if (nums[i]<privot) {
                swap(nums[j+1],nums[i]);
                j++;
            }
        }
        swap(nums[l],nums[j]);
        return j;
    }
};


vector<int> bucketSort(vector<int>& nums) {
  int buckNum = 751;
  vector<int> ans(buckNum,0);
  ans.resize(buckNum);
  for (auto& num : nums) {
    ans[num]++;
  }
  for (int i=0;i<buckNum;i++) {
    while(ans[i]>0) {
      cout << i << " " <<endl;
      ans[i]--;
    }
  }
  return ans;
}
int main() {
  vector<int> nums{1,2,10,0,9,8,8};
  int k = 4;
  vector<int> res = Solution3().findKthLargest(nums,k);
  for (auto elem : res) {
    cout << elem << endl;
  }
  nums = {30,0,180,90,730,200};
  bucketSort(nums);
  return 0;
}