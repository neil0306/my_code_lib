#include <iostream>
using namespace std;

class Solution {
    public:
        vector<int> sortedSquares(vector<int>& nums) {
            int left = 0;
            int right = nums.size() -1;
            vector<int> res(nums.size());

            for(int i = res.size()-1; i >=0; i--){
                // 计算平方
                int temp_l = nums[left] * nums[left];
                int temp_r = nums[right] * nums[right];

                if(temp_l > temp_r){
                    res[i] = temp_l;
                    left++;
                }
                else{
                    res[i] = temp_r;
                    right--;
                }
            }
            return res;
        }

        void show(const vector<int> &res)
        {
            for(size_t i = 0; i < res.size();i++){
                cout << res[i] << " ";
            }
            cout << endl;
        }
};

int main(void)
{
    Solution s;
    vector<int> nums = {-7, -3, 2, 3, 11};
    vector<int> res;

    res = s.sortedSquares(nums);
    s.show(res);

    nums = {-4,-1,0,3,10};
    s.show(s.sortedSquares(nums));
    return 0;
}