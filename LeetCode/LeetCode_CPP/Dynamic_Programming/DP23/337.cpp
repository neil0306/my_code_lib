#include <iostream>
#include <map>
using namespace std;

struct TreeNode
{
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr){}
    TreeNode(int x) : val(x),left(nullptr), right(nullptr){}
    TreeNode(int x, TreeNode* l, TreeNode* r): val(x), left(l), right(r){}
};

class Solution
{
    // 动态规划
    vector<int> robTree(TreeNode * cur)  // 传入当前需要遍历的节点，返回当前层的 DP 数组
    {
        // 递归停止条件
        if(cur == nullptr){
            return vector<int>{0,0};  // 返回 DP 数组，长度为2，偷和不偷当前节点，最大的金钱数量都是0 （因为此时遍历到了树的最底层）
        }

        // 递归处理 （遍历顺序）
        // 左：
        vector<int> left_dp = robTree(cur->left);
        // 右：
        vector<int> right_dp = robTree(cur->right);
        // 中：
        int val1 = cur->val + left_dp[0] + right_dp[0];  // 偷当前节点 + 不偷左孩子 + 不偷右孩子
        int val0 = max(left_dp[0], left_dp[1]) + max(right_dp[0], right_dp[1]); // 不偷当前节点 + [体现DP思想的地方] 根据左右孩子的状态来决定偷不偷左右孩子

        return vector<int>{val0, val1};  // 放入的顺序也要注意， 因为下标是有实际意义的！
    }

    int rob(TreeNode* root)
    {
        vector<int> result = robTree(root);
        return max(result[0], result[1]);   // 到根节点的时候返回偷到金钱最多的case
    }

    // 记忆递归
    unordered_map<TreeNode *, int> umap;    // key是节点，value存放对于【当前节点处理之后】偷到的最大金钱数
    int rob(TreeNode * root)
    {
        if(root == nullptr)     // 空(子)树
            return 0;
        if(root->left == nullptr && root->right == nullptr){  // 子树没有孩子节点
            return root->val;
        } 
        if(umap[root] != 0){
            return umap[root];
        }

        // 偷当前节点
        int val1 = root->val;
        if(root->left != nullptr){
            val1 += rob(root->left->left) + rob(root->left->right);
        }
        if(root->right != nullptr){
            val1 += rob(root->right->left) + rob(root->right->right);
        }

        // 不偷当前节点
        int val0 = rob(root->left) + rob(root->right);   // 偷左或者右（甚至两个孩子都偷了）
        umap[root] = max(val0, val1);  // 记录处理完当前节点最多能偷到多少钱
        return  max(val0, val1);
    }
};


