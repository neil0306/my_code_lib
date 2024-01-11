#include <iostream>
using namespace std;

class Solution
{
    public:
        vector<vector<int>> fourSum(vector<int>& nums, int target) 
        {
            sort(nums.begin(), nums.end());
            // 优化: 如果排序后的第一个元素就大于target, 并且当前元素已经大于等于0, 求和没有了变小的可能, 直接结束
            if(nums[0] > target && nums[0] >= 0){
                return {};
            }

            int n = nums.size();
            vector<vector<int>> res;
            for(int i = 0; i < n - 3; ++i){
                // -------- 处理第一个元素 -----------
                long a = nums[i];                                // 暂时用long类型, 因为求和会溢出
                if(i > 0 && a == nums[i-1])
                    continue;
                
                // 优化: 如果按当前顺序取到的四元组不满足条件, 则结束循环
                if(a + nums[i+1] + nums[i+2] + nums[i+3] > target){
                    break;
                }

                // 优化: 如果当前数字与倒数那三个数字(最大的三个)加起来不满足条件, 则直接进入下一次循环
                if(a + nums[n-3] + nums[n-2] + nums[n-1] < target){
                    continue;
                }

                // --------- 处理第二个元素 ------------
                for (int j = i+1; j < n - 2; ++j){
                    int b = nums[j];
                    if(a + b > target && b >= 0){               // 前两个已经不满足四元组条件
                        break;
                    }
                    if(j > i+1 && nums[j] == nums[j-1]){        // 去重
                        continue;
                    }
                    if(a + b + nums[j+1] + nums[j+2] > target){ // 同上
                        break;
                    }
                    if(a + b + nums[n-2] + nums[n-1] < target){ // 同上
                        continue;
                    }
                    
                    // ------- 处理 第三, 第四个元素 ----
                    int left = j+1, right = n-1;
                    while(left < right){
                        long s = (long)a + b + nums[left] + nums[right];     // 不用long类型的话会溢出
                        if(s > target){
                            right--;
                        }
                        else if(s < target){
                            left++;
                        }
                        else{
                            // 找到一组目标
                            res.push_back({(int)a, b, nums[left], nums[right]});  // 前面a用了long, 这里转回int

                            // 第三个元素去重
                            while(left < right && nums[left] == nums[left+1]){
                                left++;
                            }
                            // 第四个元素去重
                            while(left < right && nums[right] == nums[right-1]){
                                right--;
                            }

                            // 更新到下一个元素位置
                            left++;
                            right--;
                        }
                    }
                }
            }
            return res;
        }
};

int main(void)
{
    vector<int> nums;
    int target;
    Solution ans;
    vector<vector<int>> res;

    nums = {1,0,-1,0,-2,2}, target = 0;
    nums = {2,2,2,2,2}, target = 8;
    nums = {0,0,0,1000000000,1000000000,1000000000,1000000000}, target = 1000000000;
    res = ans.fourSum(nums, target);
    return 0;
}