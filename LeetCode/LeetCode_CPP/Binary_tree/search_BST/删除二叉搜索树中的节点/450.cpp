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
    TreeNode* deleteNode(TreeNode* root, int key) 
    {
        // 递归停止条件
        if(!root){
            return nullptr;  // 空子树, 同时也表明一路找下来都没有遇到等于key的节点
        }
        if(root->val == key){ // 找到待删除节点，分情况讨论
            if(root->left == nullptr && root->right == nullptr){  // case2: 叶子节点，删完之后这个位置就是null了
                return nullptr;
            }
            else if(root->left == nullptr && root->right != nullptr){ // case3: 只有右孩子非空
                return root->right;
            }
            else if(root->left != nullptr && root->right == nullptr){ // case4: 只有左孩子非空
                return root->left;
            }
            else{   // case5
                TreeNode * cur = root->right;
                while(cur->left != nullptr){
                    cur = cur->left;        // 找右子树的最左侧节点
                }
                cur->left = root->left;     // 将被删节点的左子树放进来
                TreeNode * tmp = root;      // 临时存一下被删节点内存地址，用来释放内存
                root = root->right;         // 右子树继位（root存放的地址变成右孩子）
                delete tmp;                 // 释放内存
                return root;                // 返回删除节点后的子树
            }
        }

        // 单次递归逻辑
        if(root->val < key){
            root->right = deleteNode(root->right, key);
        }
        else {
            root->left = deleteNode(root->left, key);
        }
        return root;
    }
};


int main(void)
{
    return 0;
}