#include <iostream>
using namespace std;

class Solution
{
    public:
        vector<int> searchRange(vector<int> &nums, int target);
        int getLeftBorder(vector<int> &nums, int target);
        int getRightBorder(vector<int> &nums, int target);
        void show(const vector<int> &res)
        {
            cout << "[" << res[0] << ", " << res[1] << "]" << endl;
        }
};

vector<int> Solution::searchRange(vector<int> &nums, int target)
{
    int leftBorder = getLeftBorder(nums, target);
    int rightBorder = getRightBorder(nums, target);

    // case1
    if (leftBorder == -2 && rightBorder == -2){
        return {-1, -1};
    }

    // case3
    if(rightBorder - leftBorder > 1){
        return {leftBorder +1, rightBorder -1};
    }
    // case2
    return {-1, -1};
}

int Solution::getLeftBorder(vector<int> &nums, int target)
{
    /* 使用左闭右闭区间 */ 
    int leftBorder = -2;
    int left = 0;
    int right = nums.size() -1;
    
    while(left <= right){
        int mid = left + (right - left) / 2;

        if(nums[mid] >= target){        // 收缩右边界, 且 nums[mid] == target 的时候也要更新 leftBorder
            right = mid - 1;
            leftBorder = right;
        }
        else{
            left = mid + 1;             // 收缩左边界
        }
    }
    return leftBorder;
}

int Solution::getRightBorder(vector<int> &nums, int target)
{
    /* 使用左闭右闭区间 */ 
    int rightBorder = -2;
    int left = 0;
    int right = nums.size() -1;

    while(left <= right){
        int mid = left + (right - left) / 2;
        if(nums[mid] <= target){     // 收缩左边界, 且 nums[mid] == target 的时候也要更新 rightBorder
            left = mid + 1;
            rightBorder = left;
        }
        else{
            right = mid - 1;
        }
    }
    return rightBorder;
}


int main(void)
{
    int target = 7;
    int test_nums[6] = {5,7,7,8,8,10};
    vector<int> nums(&test_nums[0], &test_nums[5]);
    Solution s;

    // 打印原数组
    cout << "Original Array: ";
    for(size_t i = 0; i < nums.size(); i++){
        cout << nums[i] << " ";
    }
    cout << endl;

    // 示例1
    vector<int> res = s.searchRange(nums, target);
    s.show(res);        // [1,2]

    // 示例2
    target = 6;
    res = s.searchRange(nums, target);
    s.show(res);        // [-1,-1]

    // 示例3
    nums = vector<int>();
    target = 0;
    res = s.searchRange(nums, target);
    s.show(res);        // [-1,-1]

    return 0;
}