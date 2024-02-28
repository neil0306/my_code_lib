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
    TreeNode* traverse(vector<int>& inorder, vector<int>& postorder) 
    {
        // 递归停止条件1: 
        if(inorder.size() == 0){        // 数组为零, 证明是空节点
            return nullptr;
        }

        // 处理中间节点
        int val = postorder[postorder.size()-1];    // 确认中间节点
        TreeNode * root = new TreeNode(val);        // 其实每一个节点都叫root..., 最后返回的时候会返回第一层递归的root

        // 递归停止条件2: 如果是叶子节点, 则可以直接返回
        if(inorder.size() == 1){
            return root;
        }

        // 以下是单次递归逻辑:
        int index;  // 找中序遍历中 val 的位置, 以切分中序数组
        for(index = 0; index < inorder.size(); ++index){
            if(inorder[index] == val){
                break;
            }
        }

        // 切分中序数组
        vector<int> in_left(inorder.begin(), inorder.begin()+index);        // 中序左
        vector<int> in_right(inorder.begin() + index + 1, inorder.end());   // 中序右

        // 切分后序数组
        vector<int> post_left(postorder.begin(), postorder.begin() + in_left.size()) ;  // 技巧: 后序左数组的大小 与 中序左数组大小 相同!!
        vector<int> post_right(postorder.begin() + in_left.size(), postorder.end() -1);     // 注意最后一个元素已经用过了

        // 递归构建子树: 注意别放错了
        root->left = traverse(in_left, post_left);
        root->right = traverse(in_right, post_right); 

        return root;
    }

    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) 
    {
        if(inorder.size() == 0 || postorder.size() == 0){
            return nullptr;
        }
        return traverse(inorder, postorder);
    }
};

int main(void)
{
    Solution ans;
    vector<int> inorder = {9,3,15,20,7};
    vector<int> postorder = {9,15,7,20,3};

    TreeNode* res;
    res = ans.buildTree(inorder, postorder);

    return 0;
}
