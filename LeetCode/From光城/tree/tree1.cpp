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
 // 450. 删除二叉搜索树中的节点（注意删除后需要重新让树有序）
  // 下面的写法极其不好理解，不如看代码随想录的 LeetCode 450 的题解。
  // TreeNode* deleteNode(TreeNode* root, int key) {
  //   if (!root) return NULL;
  //   if (root->val == key) {   // 找到待删除节点
  //     if (!root->left) {
  //       return root->right;   // 左子树为空，返回右子树
  //     } else if (!root->right) {
  //       return root->left;    // 右子树为空，返回左子树
  //     } else {  // 左右子树都不为空
  //       // method1
  //       // TreeNode* successor = new TreeNode(min(root->right)->val);   // 这里的min指的是【往树的左侧走，找最小节点】，因为是搜索树，有序
  //       // successor->left = root->left;
  //       // successor->right = deleteMin(root->right);
  //       // return successor;
  //       // method2
  //       TreeNode* precessor = new TreeNode(max(root->left)->val);    // 这里的max指的是【往树的右侧走，找最大节点】，因为是搜索树，有序
  //       precessor->left = deleteMax(root->left);
  //       precessor->right = root->right;
  //       return precessor;
  //       // method3
  //       // int minVal = min(root->right)->val;
  //       // root->val = minVal;
  //       // root->right = deleteNode(root->right, minVal);
  //     }
  //   } else if (root->val > key) {   // 目标节点比当前节点大，往左继续搜索
  //     root->left = deleteNode(root->left, key);
  //   } else {   // 目标节点比当前节点小，往右继续搜索
  //     root->right = deleteNode(root->right, key);
  //   }
  //   return root;
  // }

  // TreeNode* min(TreeNode* root) {
  //   if (!root->left) return root;
  //   return min(root->left);
  // }

  // TreeNode* deleteMin(TreeNode* root) {
  //   if (!root->left) return root->right;
  //   root->left = deleteMin(root->left);
  //   return root;
  // }

  // TreeNode* max(TreeNode* root) {
  //   if (!root->right) return root;
  //   return max(root->right);
  // }

  // TreeNode* deleteMax(TreeNode* root) {
  //   if (!root->right) return root->left;
  //   root->right = deleteMax(root->right);
  //   return root;
  // }

  // // 非递归删除
  // TreeNode* deleteMin(TreeNode* root) {
  //   if (root->left == NULL) return root->right;
  //   TreeNode *p = root, *pre = p;
  //   while (p->left) {
  //     pre = p;
  //     p = p->left;
  //   }
  //   pre->left = p->right;
  //   return root;
  // }
  // // 非递归返回最小 （找子树最左侧节点，在BST中等价于找最小值）
  // TreeNode* min(TreeNode* root) {
  //   TreeNode* p = root;
  //   while (p->left) {
  //     p = p->left;
  //   }
  //   return p;
  // }

  // 代码随想录中 LeetCode 450. 给的题解
  TreeNode* deleteNode(TreeNode* root, int key) 
  {
      // 递归停止条件
      if(!root){
          return nullptr;  // 空子树, 同时也表明一路找下来都没有遇到等于key的节点
      }
      if(root->val == key){ // 找到待删除节点，分情况讨论
          if(root->left == nullptr && root->right == nullptr){  // case2: 叶子节点，删完之后这个位置就是null了
              delete root;        // 包含了根节点为待删除节点的情况
              return nullptr;
          }
          else if(root->left == nullptr && root->right != nullptr){ // case3: 只有右孩子非空
              TreeNode * tmp = root->right;
              delete root;        // 释放内存
              return tmp;
          }
          else if(root->left != nullptr && root->right == nullptr){ // case4: 只有左孩子非空
              TreeNode * tmp = root->left;
              delete root;
              return tmp;
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

  // 插入: 按照BST特性去遍历，找到null的位置就插入新节点即可
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
  // 来自代码随想录的思路：
    // 一种使用额外空间的做法是：按照中序遍历的顺序遍历二叉搜索树，遍历的结果存放到一个数组，然后验证这个数组是不是递增序列即可。
    // 另一种不使用额外空间的做法：用一个pre指针指向前一个节点，按照中序遍历顺序遍历BST（遍历的时候，pre最开始会等于最左下节点，也就是树的最小值），
    //                        之后，只需要判断 cur 是不是一直都比 pre 大即可。

// 剑指 Offer 68 - I. 二叉搜索树的最近公共祖先
// https://leetcode-cn.com/problems/er-cha-sou-suo-shu-de-zui-jin-gong-gong-zu-xian-lcof/
// 剑指 Offer 68 - II. 二叉树的最近公共祖先
// https://leetcode-cn.com/problems/er-cha-shu-de-zui-jin-gong-gong-zu-xian-lcof/
  // 核心思想：遇到p或者q就返回这个节点，为空节点也返回；最后只需要遇到某个节点左右子树返回的节点都不为空，这个节点就是公共祖先
class Solution {
 public:
  TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root) return NULL;
    if (root == p || root == q) return root;
    TreeNode* left = lowestCommonAncestor(root->left, p, q);
    TreeNode* right = lowestCommonAncestor(root->right, p, q);
    if (left && right) return root;
    if (left) return left;    // 继续往上层传递公共祖先或遇到的目标节点p or q
    if (right) return right;  // 继续往上层传递公共祖先或遇到的目标节点p or q
    return NULL;
  }
};