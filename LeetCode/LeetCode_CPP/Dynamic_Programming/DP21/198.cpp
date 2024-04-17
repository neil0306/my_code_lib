#include <iostream>
#include <vector>

using namespace std;

class Solution
{
    public:
        int rob(vector<int>& nums)
        {
            int n = nums.size();
            if(n == 1){
                return nums[0];
            }

            // 初始化dp数组
            vector<int> dp(n,0);
            dp[0] = nums[0];
            dp[1] = max(nums[0], nums[1]);

            // 遍历
            for(int i = 2; i < n; ++i){
                dp[i] = max(nums[i] + dp[i-2], dp[i-1]);
            }

            return dp[n-1];
        }
};

int main(void)
{
    Solution ans;
    vector<int> nums = vector<int>{2,7,9,3,1};
    int res = ans.rob(nums);
    return res;
}