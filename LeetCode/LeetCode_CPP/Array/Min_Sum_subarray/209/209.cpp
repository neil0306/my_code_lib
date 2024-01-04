#include <iostream>

using namespace std;

class Solution
{
    public:
        int minSubArrayLen(int target, vector<int>&nums)
        {
            int sum = 0;
            int i = 0, j = 0;               // 窗口左边界
        
            // 题目中提到最长是10^5
            int tempLen = 0;              // 记录窗口宽度  
            int res = INT32_MAX;            // 最终结果

            // 窗口右边界 & 移动方式
            for (int j = 0; j < nums.size(); j++){
                // 窗口内容
                sum += nums[j];             // 每遍历一个元素就加入窗口内, 窗口里放的是元素之和

                // 窗口左边界 & 移动方式
                while(sum >= target){       // 能进while, 说明窗口元素满足条件 
                    tempLen = (j - i + 1);   // 计算当前子序列长度
                    res = res < tempLen ? res : tempLen;

                    // debug
                    show_subArr(nums, i, j);

                    // 记录好当前子序列之后, 开始尝试缩小窗口, 如果仍满足窗口条件, 会再次进入while
                    sum -= nums[i];
                    i++;
                }
            }
            return res == INT32_MAX ? 0 : res;
        }

        // for debug
        void show_subArr(vector<int> &nums, int i, int j)
        {
            cout << "[" << i << ", " << j << "]: ";
            while(i <= j){
                cout << nums[i++] << " ";
            }
            cout << endl;
        }
};

int main(void)
{
    Solution res;
    int target = 7;
    vector<int> nums = {2,3,1,2,4,3};               // res = 2
    // nums = {1,4,4}, target = 4;                     // res = 1
    // nums = {1,1,1,1,1,1,1,1}, target = 11;       // res = 0

    cout << res.minSubArrayLen(target, nums) << endl;
    return 0;
}