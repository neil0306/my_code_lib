// [两数之和] 给出一个数组nums，一个值k，找出数组中的两个下标 i，j 使得 nums[i] + nums[j] = k.
#include <iostream>
#include <vector>
#include <map>

using namespace std;

// 解法1: 用字典的方式求解，字典的key为当前元素，value保存元素的下标
vector<int> twoSum1(vector<int>&nums, int target)
{
    int n = nums.size();
    vector<int> res;
    map<int, int> tmp;

    for (int i = 0; i < n; ++i){
        if(tmp.count(target - nums[i])){   // 如果字典的key里有 target - nums[i]，count会返回1
            res.push_back(i);
            res.push_back(tmp[target - nums[i]]);
        }
        else{
            tmp[nums[i]] = i;  // 记录当前元素
        }
    }
    return res;
}


// 解法2: 复制一份数组进行排序，然后强行二分法。。。O(n^2)
vector<int> twoSum2(vector<int> &nums, int target)
{
    vector<int> res;

    vector<int> tmp_nums = nums;
    sort(tmp_nums.begin(), tmp_nums.end()); // 对复制的数组进行排序

    int l = 0;
    int r = nums.size() -1;

    while(l < r){
        if(tmp_nums[l] + tmp_nums[r] == target){  // 找到元素，然后回原数组找下标
            for (int i = 0; i < nums.size(); ++i){
                if(nums[i] == tmp_nums[l] || nums[i] == tmp_nums[r]){
                    res.push_back(i);
                }
            }
            break;
        }
        else if(tmp_nums[l] + tmp_nums[r] > target){
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
    int target = 0;
    vector<int> nums = vector<int>{-2, -3, -4, 0, 2, 6, 7, 100};
    vector<int> res1 = twoSum1(nums, target);
    vector<int> res2 = twoSum2(nums, target);

    cout << "res1: ";
    for(auto i : res1){
        cout << i << " ";
    }
    cout << endl;

    cout << "res2: ";
    for(auto i : res2){
        cout << i << " ";
    }
    cout << endl;
    return 0;
}