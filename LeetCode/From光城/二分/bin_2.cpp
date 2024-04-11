//一个数据先递增再递减，找出数组不重复的个数，比如 [1, 3, 9, 1]，结果为3，不能使用额外空间，复杂度o(n)

#include <iostream>
#include <vector>
using namespace std;

// 核心思路： 缩小区间时，往两边元素相等的方向走
int diffNum(vector<int> &nums)
{
    int ans = 0;
    int l = 0;
    int r = nums.size() -1;

    while(l <= r){
        if(nums[l] == nums[r]){
            ans++;
            int temp = nums[l];
            while(l <= r && nums[l] == temp){
                l++;
            }
            while(l <= r && nums[r] == temp){
                r--;
            }
        }
        else if (nums[l] > nums[r]){  // nums[l]比较大，nums[r]要增大才能让两者相等
            ans++;
            int temp = nums[r];
            while(l <=r && nums[r] == temp){
                r--;
            }
        }
        else{
            ans++;
            int temp = nums[l];
            while(l <= r && nums[l] == temp){
                l++;
            }
        }
    }
    return ans;
} 

int main(void)
{
    vector<int> nums = vector<int>{1, 3, 9, 1};
    cout << diffNum(nums) << endl;
    return 0;
}