/*
    给定一个排序数组，平方后，数组当中有多少不同的数字（相同算一个）。
*/

#include <iostream>
#include <vector>
using namespace std;

int diffSquareNum(vector<int> &nums)
{
    int n = nums.size();

    int l = 0;
    int r = n-1;
    int ans = 0;

    while(l <= r){
        // case 1: nums[l] + nums[r] == 0
        if(nums[l] + nums[r] == 0){
            ans++;  // 记录当前case的结果
            int temp = nums[r];
            while(l <= r && nums[l] == -temp){
                l++;
            }
            while(l <= r && nums[r] == temp){
                r--;
            }
        } 
        // case 2: nums[l] + nums[r] > 0； 即 abs(nums[l]) < abs(nums[r])
        else if(nums[l] + nums[r] > 0){
            ans++;  // 记录当前 nums[r] 作为一个新结果
            int temp = nums[r];
            while(l <= r && nums[r] == temp){   // 跳过当前元素，如果有相同的，则继续跳过
                r--;
            }
        }
        // case 3: nums[l] + nums[r] < 0; 即 abs(nums[l] > nums[r])
        else{
            ans++;
            int temp = nums[l];
            while (l <= r && nums[l] == temp){
                l++;
            }
        }
    }

    return ans;
}

int main(void)
{
    vector<int> nums = vector<int> {-3, -3, -2, -1, 0, 0, 0, 1, 4, 5, 6};
    cout << diffSquareNum(nums) << endl;
    return 0;
}