// 递增数组，找出和为k的数对

#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> findPair(vector<int> &nums, int k) 
{
    vector<vector<int>> res;
    if(nums.size() < 2){   // 数字不足
        return res;
    }

    int l = 0;
    int r = nums.size()-1;

    while(l < r){
        if(nums[l] + nums[r] == k){
            res.push_back({nums[l++], nums[r--]});

        }
        else if(nums[l] + nums[r] > k){
            r--;
        }
        else{
            l++;
        }
    }
    return res;
}


int main(void)
{
    vector<int> nums = vector<int>{-3,-2,-1,0,1,2,3};
    vector<vector<int>> res = findPair(nums, 0);
    for (auto i : res){
        for(auto j : i){
            cout << j << " ";
        }
    }
    cout << endl;
    return 0;
}


