#include <iostream>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};


class Solution {
public:
    TreeNode * traversal(TreeNode * cur, TreeNode* p, TreeNode * q)  // 返回：最近的公共祖先； 输入：当前遍历节点，p，q节点
    {
        // 递归停止条件
        if(!cur){
            return cur;  // 相当于返回nullptr （由于题目限制，其实一定会有公共祖先，这个条件不写也可以）
        }

        // 单次递归逻辑 (注意这里只是搜索【一条边】而非整棵树)
        if (cur->val > p->val && cur->val > q->val){            // 左
            TreeNode * left = traversal(cur->left, p, q);
            if(left){
                return left;    // 如果left不是空的，则说明在cur的左子树里找到了最近公共祖先
            }
        }
        
        if (cur->val < p->val && cur->val < q->val){            // 右
            TreeNode * right = traversal(cur->right, p, q);
            if(right){
                return right;
            }
        }
        
        // 中：包含了cur介于p和q之间的情况，以及cur为p或者q的情况（[p,q]区间之外的情况）
        return cur;
    }

    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        TreeNode * res = traversal(root, p, q);
        return res;
    }
};

int main(void)
{
    return 0;
}