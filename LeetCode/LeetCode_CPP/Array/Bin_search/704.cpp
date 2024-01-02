/*
给定一个 n 个元素有序的（升序）整型数组 nums 和一个目标值 target  ，写一个函数搜索 nums 中的 target，如果目标值存在返回下标，否则返回 -1。

    示例 1:

    输入: nums = [-1,0,3,5,9,12], target = 9
    输出: 4
    解释: 9 出现在 nums 中并且下标为 4
    示例 2:

    输入: nums = [-1,0,3,5,9,12], target = 2
    输出: -1
    解释: 2 不存在 nums 中因此返回 -1


    提示：
    你可以假设 nums 中的所有元素是不重复的。
    n 将在 [1, 10000]之间。
    nums 的每个元素都将在 [-9999, 9999]之间。
*/

#include <vector>
#include <iostream>
using namespace std;

class search1
{
    public:
        // method 1: 左必右闭区间
        int search(vector<int> &nums, int target)
        {
            int left = 0;                   // 左指针
            int right = nums.size() - 1;    // 右指针
            while (left <= right){          // 左闭右闭区间
                int middle = left + ((right - left) / 2);  // 防止溢出
                if (nums[middle] > target){  // target 在左侧区间
                    right = middle - 1;      // 搜索区间更新为： [left, middle -1]
                }
                else if (nums[middle] < target){ // target 在右侧区间
                    left = middle + 1;      // 搜索区间更新为：[middle + 1, right]
                }
                else{
                    return middle;
                }
            }
            return -1;
        }
};

int main(void)
{
    search1 test;
    int num_test[6] =  {-1,0,3,5,9,12};
    std::vector<int> nums(&num_test[0], &num_test[5]);

    int target = 2;
    cout << "Searching for 2: "; 
    cout << test.search(nums, target) << endl;

    target = 9;
    cout << "Searching for 9: "; 
    cout << test.search(nums, target) << endl;
}
