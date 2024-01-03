#include <iostream>
using namespace std;

class Solution
{
    public:
        void moveZeros1(vector<int> &nums)
        {
            if(nums.size() <=1){
                return;
            }

            int slow = 0, fast = 0;
            // 找到数组中第一个0元素的位置
            while(fast < nums.size() && nums[fast] != 0){
                fast++;
            }

            if(fast < nums.size()){
                slow = fast;                // 把slow移动到数组中第一个零元素的位置

                while(fast < nums.size()){  // 确保遍历完所有元素
                    while(fast < nums.size() && nums[fast] == 0){   // 在保证不越界的前提下, 找非零元素的位置
                        fast++;
                    }
                    if(fast < nums.size()){                         // 将非零元素与slow位置元素交换
                        // int temp = nums[slow];
                        // nums[slow++] = nums[fast];
                        // nums[fast] = temp;
                        nums[slow++] = nums[fast];
                        nums[fast++] = 0;
                    }
                }
            }
        }

        void moveZeros2(vector<int> &nums)      // 简洁写法, 但是元素操作的 次数 相对写法1来说比较多. 每个元素至多遍历两次
        {
            int slow = 0;
            int fast = 0;
            while(fast < nums.size()){
                if(nums[fast] != 0){            // 只要fast指向的元素不是零, 就交换
                    swap(nums[slow], nums[fast]);
                    slow++;
                }
                fast++;
            }
        }
};

int main(void)
{
    Solution s;
    // vector<int> nums = {0,1,0,3,12};
    // vector<int> nums = {2,0,0,0,0,0,1,3,0,3,12,0};
    vector<int> nums = {0,0,0,0,0,0,1};
    // vector<int> nums = {1,1};


    s.moveZeros2(nums);

    cout << "results: ";
    for(size_t i = 0; i < nums.size(); i++){
        cout << nums[i] << " ";
    }
    cout << endl;

    return 0;
}