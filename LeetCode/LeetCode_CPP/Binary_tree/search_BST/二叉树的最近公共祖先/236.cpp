#include <iostream>
using namespace std;

struct TreeNode
{
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr){}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr){}
    TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r){}
};

class Solution
{
    public:
        TreeNode* traversal(TreeNode* root, TreeNode* p, TreeNode* q)
        {
            // 递归终止条件（为了清晰，分开写）
            if(!root){
                return nullptr;        // 此时相当于返回了null
            }
            if(root == p || root == q){
                return root;        // 此时返回了p或者q
                                    // 这个操作顺便把【公共祖先为q或者p】的情况处理好了
            }

            // 单次递归逻辑
            TreeNode * left = traversal(root->left, p, q);      // 左
            TreeNode * right = traversal(root->right, p, q);    // 右
            if(left != nullptr && right != nullptr){  // 同时找到了p和q
                return root;   // 找到公共祖先节点，返回该节点 ()
            }
            // 下面的if语句是为了将公共祖先顺利传到根节点 （左右子树有一个不为空，就返回非空节点）
            if(left == nullptr && right != nullptr){
                return right;  
            }
            else if (left != nullptr && right == nullptr){
                return left;
            }
            else{
                return nullptr;     // 左右都是空，没有找到公共祖先
            }
        }

        TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q)
        {
            TreeNode* res = traversal(root, p, q);
            return res;
        }

};

int main(void)
{
    
    return 0;
}