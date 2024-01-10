#include <iostream>

using namespace std;

class Solution
{
    public:
        // 用哈希表
        vector<int> intersection1(vector<int>& nums1, vector<int>& nums2) 
        {
            if(0 == nums1.size() || 0 == nums2.size()){
                return {};
            }
            
            vector<int> res;
            unordered_map<int, int> temp1, temp2; 
            for(size_t i = 0; i < nums1.size(); ++i){
                temp1[nums1[i]] = 1;
            }
            for(size_t j = 0; j < nums2.size(); ++j){
                if(temp1[nums2[j]] == 1){
                    temp2[nums2[j]] = 1;
                }
            }

            for(auto item : temp2){
                res.push_back(item.first);
            }
            return res;
        }

        // 用数组...
        vector<int> intersection(vector<int>& nums1, vector<int>& nums2) 
        {
            if(0 == nums1.size() || 0 == nums2.size()){
                return {};
            }

            int nums[1001] = {0};
            vector<int> res;

            for(size_t i = 0; i < nums1.size(); ++i){
                nums[nums1[i]] = 1;
            }
            for(int j=0; j< nums2.size(); j++){
                if(nums[nums2[j]]==1){
                    res.push_back(nums2[j]);
                    nums[nums2[j]]++;           // 防止存在重复输出
                }
            }
            return res;
        }
};

int main(void)
{
    vector<int> nums1, nums2, res;
    Solution ans;

    nums1 = {1,2,2,1}, nums2 = {2,2};
    nums1 = {4,9,5}, nums2 = {9,4,9,8,4};
    // nums1 = {}, nums2 = {9,4,9,8,4};

    res = ans.intersection(nums1, nums2);
    
    for(size_t i = 0; i < res.size(); i++){
        cout << res[i] << " ";
    }
    cout << endl;
    return 0;
}