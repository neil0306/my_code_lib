#include <iostream>
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
    TreeNode* insertIntoBST(TreeNode* root, int val) 
    {
        // 递归停止条件：遍历到叶子节点的时候就插入新节点
        if(root == nullptr){
            TreeNode * node = new TreeNode(val);
            return node;   // 具体是左孩子还是右孩子，由递归决定
        }

        // 左
        if(root->val > val){
            root->left = insertIntoBST(root->left, val);    // 遍历&更新左子树
        }
        if(root->val < val){
            root->right = insertIntoBST(root->right, val);  // 遍历&更新右子树
        }
        return root;
    }
};

