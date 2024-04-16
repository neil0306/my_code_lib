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

class Solution
{
    public:
        TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) 
        {
            if(!preorder.size() || !inorder.size()){
                    return nullptr;
            }

            TreeNode* root = new TreeNode(preorder[0]); // 【中】获得根节点

            // 找中序遍历中根节点位置
            int j = 0;
            for (; j < inorder.size(); ++j){
                if (inorder[j] == root->val){
                    break;
                }
            }

            // 划分区间
            vector<int> pre_left = vector<int>(preorder.begin()+1, preorder.begin() + j + 1);  // 左闭右开 [1, j+1)，left一共j个元素
            vector<int> pre_right = vector<int>(preorder.begin() + j + 1, preorder.end());   // 左闭右开 [j, end]
            vector<int> in_left = vector<int>(inorder.begin(), inorder.begin() + j);  // 左闭右开 [0, j), 注意j是中间节点
            vector<int> in_right = vector<int>(inorder.begin() + j + 1, inorder.end());

            root->left = buildTree(pre_left, in_left);
            root->right = buildTree(pre_right, in_right);

            return root;
        }
};

int main(void)
{
    return 0;
}