#include <iostream>

using namespace std;

struct TreeNode 
{
    int val;
    TreeNode * left;
    TreeNode * right;
    TreeNode(int v) : val(v), left(nullptr), right(nullptr){}
};

class Solution
{
    public:
        void order(TreeNode* cur, vector<vector<int>> result, int depth)
        {
            if(cur == nullptr){
                return;
            }

            if(result.size() == depth){
                result.push_back(vector<int>());
            }

            result[depth].push_back(cur->val);

            order(cur->left, result, depth + 1);
            order(cur->right, result, depth + 1);
        }

        vector<vector<int>> levelOrder(TreeNode * root)
        {
            vector<vector<int>> result;
            int depth = 0;
            order(root, result, depth);
            return result;
        }

};

int main(void)
{
    Solution s;
    vector<vector<int>> res;


    return 0;
}