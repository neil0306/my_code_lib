#include <iostream>
using namespace std;

class Solution
{
    public:
        int fourSumCount1(vector<int>& nums1, vector<int>& nums2, vector<int>& nums3, vector<int>& nums4) 
        {
            int result = 0;
            unordered_map<int,int> hashTable_12;

            // 统计 nums1 和 nums2 可能出现的 "两数之和" 以及对应次数
            for(int i = 0; i < nums1.size(); ++i){
                for(int j = 0; j < nums2.size(); ++j){
                    hashTable_12[nums1[i] + nums2[j]]++;
                }
            }

            // 从 nums3 和 nums4 中求两数之和, 
            // 然后去 hashTable_12 中找有没有对应的相反数, 
            // 如果有, 则计入result
            for(int i = 0; i < nums3.size(); ++i){
                for(int j = 0; j < nums4.size(); ++j){
                    int temp = nums3[i] + nums4[j];
                    if( hashTable_12.find(-temp) != hashTable_12.end()){
                        result += hashTable_12[-temp];
                    }
                }
            }
            return result;
        }

        // 优化一下操作步骤细节 (接近200毫秒)
        int fourSumCount2(vector<int>& nums1, vector<int>& nums2, vector<int>& nums3, vector<int>& nums4) 
        {
            int result = 0;
            unordered_map<int,int> hashTable_12;
            for(int nA : nums1){
                for(int nB : nums2){
                    hashTable_12[nA + nB]++;
                }
            }

            for(int nC : nums3){
                for(int nD : nums4){
                    if(hashTable_12.find( -(nC+nD) ) != hashTable_12.end() ){
                        result += hashTable_12[ -(nC+nD) ];
                    }
                }
            }
            return result;
        }

};

int main(void)
{
    Solution ans;
    vector<int> nums1, nums2, nums3, nums4;

    nums1 = {1,2}, nums2 = {-2,-1}, nums3 = {-1,2}, nums4 = {0,2};
    nums1 = {0}, nums2 = {0}, nums3 = {0}, nums4 = {0};
    
    cout << ans.fourSumCount2(nums1, nums2, nums3, nums4) << endl;
    return 0;
}