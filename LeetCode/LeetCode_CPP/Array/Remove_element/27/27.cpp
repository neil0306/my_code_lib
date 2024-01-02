/*
    Leetcode链接: https://leetcode.cn/problems/remove-element/
*/

#include <iostream>

using namespace std;

class Solution
{
    public:
        int remove_elem(vector<int> &nums, int val)
        {
            int fast_p = 0, slow_p = 0;  // 快慢指针, 它们的起点相同
            while(fast_p < nums.size()){
                // 当快指针不等于被删除元素时才覆盖掉慢指针元素
                if(nums[fast_p] != val){
                    nums[slow_p++] = nums[fast_p];      // 替换掉元素之后, 慢指针才移动
                }
                fast_p++;                               // 每次循环, 快指针都会移动
            }
            return slow_p;
        }

        void show_arr(vector<int> &nums, int len)
        {
            for(size_t i = 0; i < len; i++){
                cout << nums[i] << " ";
            }
            cout << endl;
        }

};

int main(void)
{
    int test_num[8] = {0,1,2,2,3,0,4,2};
    int val = 2;
    vector<int> nums(test_num, test_num+8);

    Solution s;
    cout << "Target: " << val << endl;
    cout << "Original array: ";
    s.show_arr(nums, nums.size());

    int res = s.remove_elem(nums, val);

    cout << "After removing: ";
    s.show_arr(nums, res);
    return 0;
}
