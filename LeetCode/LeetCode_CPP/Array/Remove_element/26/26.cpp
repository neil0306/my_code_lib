#include <iostream>

using namespace std;

class Solution
{
    public:
        int removeDuplicates(vector<int>& nums)
        {
            if(nums.size() <= 1){
                return nums.size();
            }
            int fast = 0, slow = 0;
            while(fast < nums.size()){
                while(fast < nums.size() && nums[fast] == nums[slow]){  // 注意要防止 fast 越界
                    fast++;
                }
                if(fast < nums.size() && nums[fast] != nums[slow]) // 注意要防止 fast 越界
                {
                    nums[++slow] = nums[fast++];
                }
            }
            return slow+1;
        }

        void show(vector<int>& nums)
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
    vector<int> nums = {0,0,1,1,1,2,2,3,3,4};

    cout << s.removeDuplicates(nums) << endl;
    s.show(nums);

    cout << " --------------- "<< endl;
    nums = {1, 1};
    cout << s.removeDuplicates(nums) << endl;
    s.show(nums);
    return 0;
}