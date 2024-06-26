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

        // 优化版:
        vector<vector<int>> threeSum_opt(vector<int> &nums) 
        {
            sort(nums.begin(), nums.end());

            // 额外优化: 如果最小的元素都大于0, 就直接结束.
            if(nums[0] > 0){
                return {};
            }

            vector<vector<int>> ans;
            int n = nums.size();
            for (int i = 0; i < n - 2; ++i) {
                // ----------- 处理 第一个元素 ------
                int x = nums[i];

                // 跳过重复数字
                if (i > 0 && x == nums[i - 1])
                    continue;

                // 优化一: 如果当前位置和待处理的最小的两个数加起来大于0, 则一定不满足条件, 直接结束
                if (x + nums[i + 1] + nums[i + 2] > 0) 
                    break;
                
                // 优化二: 如果当前数字和最后两个数子加起来不满足条件, 则继续遍历下一个元素, 以增大求和
                if (x + nums[n - 2] + nums[n - 1] < 0)
                    continue;
                
                // ------- 处理第二, 第三个元素 --------
                int left = i + 1, right = n - 1;
                while (left < right) {
                    int s = x + nums[left] + nums[right];
                    if (s > 0) --right;
                    else if (s < 0) ++left;
                    else {
                        ans.push_back({x, nums[left], nums[right]});
                        for (++left; left < right && nums[left] == nums[left - 1]; ++left); // 跳过重复数字
                        for (--right; right > left && nums[right] == nums[right + 1]; --right); // 跳过重复数字
                    }
                }
            }
            return ans;
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