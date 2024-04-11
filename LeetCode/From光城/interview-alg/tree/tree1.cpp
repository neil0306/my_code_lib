/*
 * @Author: gcxzhang
 * @Date: 2020-06-29 19:08:31
 * @LastEditTime: 2020-09-30 10:58:31
 * @Description: 树相关
 * @FilePath: /alg/alg/tree/tree1.cpp
 */
#include <iostream>
using namespace std;
struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
 public:
  TreeNode* deleteNode(TreeNode* root, int key) {
    if (!root) return NULL;
    if (root->val == key) {
      if (!root->left) {
        return root->right;
      } else if (!root->right) {
        return root->left;
      } else {
        // method1
        // TreeNode* successor = new TreeNode(min(root->right)->val);
        // successor->left = root->left;
        // successor->right = deleteMin(root->right);
        // return successor;
        // method2
        TreeNode* precessor = new TreeNode(max(root->left)->val);
        precessor->left = deleteMax(root->left);
        precessor->right = root->right;
        return precessor;
        // method3
        // int minVal = min(root->right)->val;
        // root->val = minVal;
        // root->right = deleteNode(root->right, minVal);
      }
    } else if (root->val > key) {
      root->left = deleteNode(root->left, key);
    } else {
      root->right = deleteNode(root->right, key);
    }
    return root;
  }
  TreeNode* min(TreeNode* root) {
    if (!root->left) return root;
    return min(root->left);
  }
  TreeNode* deleteMin(TreeNode* root) {
    if (!root->left) return root->right;
    root->left = deleteMin(root->left);
    return root;
  }
  TreeNode* max(TreeNode* root) {
    if (!root->right) return root;
    return max(root->right);
  }
  TreeNode* deleteMax(TreeNode* root) {
    if (!root->right) return root->left;
    root->right = deleteMax(root->right);
    return root;
  }
  // 非递归删除
  TreeNode* deleteMin(TreeNode* root) {
    if (root->left == NULL) return root->right;
    TreeNode *p = root, *pre = p;
    while (p->left) {
      pre = p;
      p = p->left;
    }
    pre->left = p->right;
    return root;
  }
  // 非递归返回最小
  TreeNode* min(TreeNode* root) {
    TreeNode* p = root;
    while (p->left) {
      p = p->left;
    }
    return p;
  }

  // 插入
  TreeNode* insertIntoBST(TreeNode* root, int val) {
    if (!root) return new TreeNode(val);

    if (root->val > val) {
      root->left = insertIntoBST(root->left, val);
    } else if (root->val < val) {
      root->right = insertIntoBST(root->right, val);
    }
    return root;
  }
  // 非递归插入
  TreeNode* insertIntoBST(TreeNode* root, int val) {
    if (!root) return new TreeNode(val);

    TreeNode *p = root, *pre = root;
    while (p) {
      pre = p;
      if (p->val > val) {
        p = p->left;
      } else if (p->val < val)
        p = p->right;
    }
    if (pre->val > val) {
      pre->left = new TreeNode(val);
    } else {
      pre->right = new TreeNode(val);
    }

    return root;
  }

  // 搜索
  TreeNode* searchBST(TreeNode* root, int val) {
    if (!root) return NULL;
    if (root->val == val)
      return root;
    else if (root->val > val)
      return searchBST(root->left, val);
    return searchBST(root->right, val);
  }
  // 非递归搜索
  TreeNode* searchBST(TreeNode* root, int val) {
    if (!root) return NULL;
    TreeNode* p = root;
    while (p) {
      if (p->val > val) {
        p = p->left;
      } else if (p->val < val) {
        p = p->right;
      } else if (p->val == val) {
        return p;
      }
    }
    return NULL;
  }

  // 98. 验证二叉搜索树 坑!!!
  bool isValidBST(TreeNode* root) { return isValidBST(root, NULL, NULL); }

  bool isValidBST(TreeNode* root, TreeNode* min, TreeNode* max) {
    if (!root) return true;
    if (min && root->val <= min->val) return false;
    if (max && root->val >= max->val) return false;
    return isValidBST(root->left, min, root) &&
           isValidBST(root->right, root, max);
  }
};

// 剑指 Offer 68 - I. 二叉搜索树的最近公共祖先
// https://leetcode-cn.com/problems/er-cha-sou-suo-shu-de-zui-jin-gong-gong-zu-xian-lcof/
// 剑指 Offer 68 - II. 二叉树的最近公共祖先
// https://leetcode-cn.com/problems/er-cha-shu-de-zui-jin-gong-gong-zu-xian-lcof/
class Solution {
 public:
  TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root) return NULL;
    if (root == p || root == q) return root;
    TreeNode* left = lowestCommonAncestor(root->left, p, q);
    TreeNode* right = lowestCommonAncestor(root->right, p, q);
    if (left && right) return root;
    if (left) return left;
    if (right) return right;
    return NULL;
  }
};