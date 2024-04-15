#include <iostream>
#include <stack>
using namespace std;

struct TreeNode
{
    int val;
    TreeNode * left;
    TreeNode * right;
    TreeNode(int n) : val(n), left(nullptr), right(nullptr){}
};

void preOrder(TreeNode* root) 
{
    stack<TreeNode*> st;
    TreeNode* p = root;
    while (p || !st.empty()) {
        if (p) {                  // 【左】每次都把left入栈
            cout << p->val << endl; // 【中】
            p = p->left;
            st.push(p);   // st最后入栈了一个空指针
        } else {                  // 【右】
            p = st.top(); 
            st.pop();
            p = p->right;
        }
    }
}

int main(void)
{
    // 创建一个示例树
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    root->right->left = new TreeNode(6);
    root->right->right = new TreeNode(7);

    preOrder(root);
    return 0;
}