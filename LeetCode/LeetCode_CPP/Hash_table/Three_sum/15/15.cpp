#include <iostream>

using namespace std;

class Solution
{
    public:
        vector<vector<int>> threeSum(vector<int>& nums) 
        {
            // 第一步先排序
            sort(nums.begin(), nums.end());
            
            // 排序后的数组如果第一个元素大于0, 则任意三数之和必不为零
            if(nums[0] > 0){
                return {};
            }

            // 遍历并寻找第一个元素
            vector<vector<int>> res;
            for(int i = 0; i < nums.size(); ++i){
                // --------- 固定第一个元素 ---------
                // 对第一个元素去重, 遇到重复的第一个元素就重新寻找
                if(i > 0 && nums[i] == nums[i-1]){
                    continue;
                }

                //  -------- 寻找第二第三个元素 ------
                int left = i+1;
                int right = nums.size()-1;

                while(left < right){
                    // 由于nums已经排序, 如果不够大, 就让left右移, 增加求和值 
                    if(nums[i] + nums[left] + nums[right] < 0){
                        left++;
                    }
                    // 由于nums已经排序, 如果太大了, 就让right右移, 减小求和值 
                    else if(nums[i] + nums[left] + nums[right] > 0){
                        right--;
                    }
                    else{
                        // 进入else, 说明找到了一个三元组
                        res.push_back({nums[i], nums[left], nums[right]});

                        // 对 nums[left] 去重, while出来之后, left仍处于同一个元素上
                        while(left < right && nums[left] == nums[left + 1]){
                            left++;
                        }
                        
                        // 对 nums[right] 去重, while出来之后, right仍处于同一个元素上
                        while(left < right && nums[right] == nums[right - 1]){
                            right--;
                        }

                        // 由于已经找到一个答案, 此时应该各自更新, 跳开这个已经用过的元素
                        left++;
                        right--;
                    }
                }
            }
            return res;
        }
};

int main(void)
{
    Solution ans;
    vector<int> nums;
    vector<vector<int>> res;

    nums = {-1,0,1,2,-1,-4};
    res = ans.threeSum(nums);
    return 0;
}