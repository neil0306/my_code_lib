#include <iostream>

using namespace std;

class Solution
{
    public:
        vector<int> twoSum(vector<int>& nums, int target) 
        {
            unordered_map<int, int> map_nums;       // 存放元素在数组中的位置
            for(int i = 0; i < nums.size(); ++i){
                // 如果在哈希表中找到了另一个元素
                if(map_nums.find(target - nums[i]) != map_nums.end()){
                    return {i, map_nums[target - nums[i]]};
                }
                
                // 如果哈希表中没有这个元素, 则把遍历过的当前元素放入哈希表
                map_nums[nums[i]] = i;
            }
            return {};
        }
};

int main(void)
{

    int target;
    Solution ans;
    vector<int> nums, res;

    nums = {3,3}, target = 6;
    res = ans.twoSum(nums, target);

    for(int item:res){
        cout << item << " ";
    }
    cout << endl;
    return 0;
}