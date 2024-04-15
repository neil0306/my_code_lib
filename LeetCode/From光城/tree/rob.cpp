/*
 * @Author: 光城
 * @Date: 2020-09-30 10:56:13
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:57:24
 * @Description: 337. 打家劫舍 III
 * https://leetcode-cn.com/problems/house-robber-iii/
 * @FilePath: /alg/alg/tree/rob.cpp
 */

/* 这道题本质上是动态规划题。 */


#include <iostream>
#include <map>
using namespace std;
struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};
class Solution {
 private:
  map<TreeNode*, int> m;

 public:
  int rob(TreeNode* root) {
    m = map<TreeNode*, int>();
    return dfs(root);
  }

  int dfs(TreeNode* root) {
    if (!root) return 0;
    if (m.count(root)) return m[root];   // 防止重复运算

    int res = 0;
    int choos_root =
        root->val +
        (root->left ? dfs(root->left->left) + dfs(root->left->right) : 0) +   // 孩子节点如果不为null，就递归，为null就为0
        (root->right ? dfs(root->right->left) + dfs(root->right->right) : 0);
    int not_choose_root = dfs(root->left) + dfs(root->right);
    return m[root] = max(choos_root, not_choose_root);  // 记录了root节点处能获得的最大金钱树，同时返回该金钱数
  }
};