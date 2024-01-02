#include <iostream>
using namespace std;

class Solution
{
    public:
        int search_inset_id(vector<int> & nums, int target)
        {
            // 区间定义为 "左闭右闭"
            int left = 0;
            int right = nums.size() - 1;
            while(left <= right){
                int mid = left + (right - left) / 2;

                if (nums[mid] > target){            // [left, mid - 1]
                    right = mid -1;
                }
                else if(nums[mid] < target){        // [mid + 1, right]
                    left = mid + 1;
                }
                else{
                    return mid;                  // case2: 目标值等于数组中的某个元素
                }
            }

            /*
                假设数组中找不到等于 target 元素时, 我们暂时返回 -1, 那么:
                    处理其它3个case:    
                        case1: 目标值在数组所有元素之前, 跳出while时的搜索区间为 [0, -1]               ===========> 新元素应放在 -1 + 1 = 0 的位置
                                因为最后一次进入while循环后, mid = 0, 走到if语句时会进入 target < nums[0] 分支, 此时 right = mid - 1 = -1

                        case3: 目标值插入数组元素之间的某个位置, 跳出while时的搜索区间为 [left, right]  ============> 新元素应放在 right + 1 位置

                        case4: 目标值在数组所有元素之后, 跳出while时的搜索区间为 [left, right]         ============> 新元素应放在 right + 1 位置            
            */
            return right + 1;
        }

    void show(vector<int> & nums)
    {
        for(size_t i = 0; i < nums.size(); i++){
            cout << nums[i] << " ";
        }
        cout << endl;
    }
};

int main(void)
{
    Solution s;
    int target = 5;
    int test_nums[] = {1,3,5,6};
    vector<int> nums(test_nums, test_nums+4);
    
    s.show(nums);
    cout << "target = " << target << endl;
    cout << "inser index: " << s.search_inset_id(nums, target) << endl;
    
    return 0;
}