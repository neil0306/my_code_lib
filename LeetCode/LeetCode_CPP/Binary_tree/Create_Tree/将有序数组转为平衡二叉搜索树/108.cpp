#include <iostream>
#include <vector>

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
public:
    TreeNode* sortedArrayToBST(vector<int>& nums) {
        if(nums.size() < 1){  // 数组为空，此为叶子节点
            return nullptr;
        }

        int n = nums.size();
        int mid = n/2;  // 中间节点

        vector<int> l_num = vector<int>(nums.begin(), nums.begin()+mid);
        vector<int> r_num = vector<int>(nums.begin()+mid+1, nums.end());
        
        TreeNode* cur = new TreeNode(nums[mid]);          // 中
        cur->left = sortedArrayToBST(l_num);              // 左
        cur->right = sortedArrayToBST(r_num);             // 右

        return cur;
    }
};


int main(void)
{
    vector<int> nums = {-10,-3,0,5,9};
    Solution ans;

    ans.sortedArrayToBST(nums);
    return 0;
}