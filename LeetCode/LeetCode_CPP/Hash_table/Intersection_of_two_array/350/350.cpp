#include <iostream>

using namespace std;

class Solution
{
    public:
        vector<int> intersect(vector<int>& nums1, vector<int>& nums2) 
        {
            if(nums1.size() == 0 || nums2.size() == 0)
                return {};
            
            unordered_map<int, int> tmp;
            vector<int> res;
            for(int i = 0; i < nums1.size(); ++i){
                tmp[nums1[i]]++;                    // 因为结果数组不去重, 所以这里需要计数
            }
            for(int i = 0; i < nums2.size(); ++i){
                if(tmp[nums2[i]] > 0){
                    res.push_back(nums2[i]);
                    tmp[nums2[i]]--;                // 防止重复放入结果, 经过--的位置要么为0, 要么为负数
                }
            }
            return res;
        }

        // 双指针法
        vector<int> intersect1(vector<int>& nums1, vector<int>& nums2) 
        {
            if(nums1.size() == 0 || nums2.size() == 0)
                return {};
            
            vector<int> res;

            // 先排序: 从小到大
            sort(nums1.begin(), nums1.end());
            sort(nums2.begin(), nums2.end());

            // 双指针法
            int p1 = 0, p2 = 0;
            while(p1 < nums1.size() && p2 < nums2.size()){      // 需要用 &&
                if(nums1[p1] < nums2[p2]){
                    p1++;
                }
                else if(nums1[p1] > nums2[p2]){
                    p2++;
                }
                else{
                    res.push_back(nums1[p1]);
                    p1++;
                    p2++;
                }
            }
            return res;
        }
};

int main(void)
{
    vector<int> nums1, nums2, res;
    Solution ans;

    nums1 = {4,9,5}, nums2 = {9,4,9,8,4};
    nums1 = {1,2,2,1}, nums2 = {2,2};
    res = ans.intersect1(nums1, nums2);

    for(size_t i = 0; i < res.size(); i++){
        cout << res[i] << " ";
    }
    cout << endl;
    return 0;
}