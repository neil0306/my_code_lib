#include <vector>
class search1
{
    public:
        // method 1: 左必右闭区间
        int search(vector<int> &nums, int target)
        {
            int left = 0;                   // 左指针
            int right = nums.size() - 1;    // 右指针
            while (left <= right){
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
    std::vector<int> nums = [-1,0,3,5,9,12];
    int target = 2;

    std::cout << test.search(nums, target);
}
