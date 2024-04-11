/*
 * @Author: gcxzhang
 * @Date: 2020-06-29 16:11:17
 * @LastEditTime: 2020-09-30 11:06:54
 * @Description: 树相关
 * @FilePath: /alg/alg/tree/tree2.cpp
 */
#include <climits>
#include <cmath>
#include <iostream>
#include <queue>
#include <stack>
using namespace std;
struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

// 100. 相同的树
// https://leetcode-cn.com/problems/same-tree/
class Solution {
 public:
  // 非递归前序遍历
  /**
   * @description:
   * @param {type}
   * @return:
   */
  bool isSameTree(TreeNode* p, TreeNode* q) {
    stack<TreeNode*> stack_p, stack_q;
    stack_p.push(p);
    stack_q.push(q);
    while (!stack_p.empty() || !stack_q.empty()) {
      TreeNode* p_node = stack_p.top();
      stack_p.pop();
      TreeNode* q_node = stack_q.top();
      stack_q.pop();

      if (!p_node && !q_node) continue;
      if (p_node && !q_node || !p_node && q_node) return false;
      if (p_node->val != q_node->val) return false;

      stack_p.push(p_node->right);
      stack_p.push(p_node->left);
      stack_q.push(q_node->right);
      stack_q.push(q_node->left);
    }
    return true;
  }

  bool isSameTree(TreeNode* p, TreeNode* q) {
    stack<TreeNode*> stack_p, stack_q;
    while (p || q || !stack_p.empty() || !stack_q.empty()) {
      if (p && q) {
        if (p->val != q->val) return false;
        stack_p.push(p);
        stack_q.push(q);
        p = p->left;
        q = q->left;
      } else if (!p && !q) {
        p = stack_p.top();
        q = stack_q.top();
        stack_p.pop();
        stack_q.pop();
        p = p->right;
        q = q->right;
      } else
        return false;
    }
    return true;
  }
  // 中序遍历
  bool isSameTree(TreeNode* p, TreeNode* q) {
    stack<TreeNode*> stack_p, stack_q;
    while (p || q || !stack_p.empty() || !stack_q.empty()) {
      if (p && q) {
        stack_p.push(p);
        stack_q.push(q);
        p = p->left;
        q = q->left;
      } else if (!p && !q) {
        TreeNode* p_node = stack_p.top();
        TreeNode* q_node = stack_q.top();
        stack_p.pop();
        stack_q.pop();
        if (p_node->val != q_node->val) return false;
        p = p_node->right;
        q = q_node->right;
      } else
        return false;
    }
  }
  bool isSameTree(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;
    stack<TreeNode*> st_p;
    stack<TreeNode*> st_q;

    TreeNode *pre_p = NULL, *pre_q = NULL;
    st_p.push(p);
    st_q.push(q);
    while (!st_p.empty() || !st_q.empty()) {
      p = st_p.top();
      q = st_q.top();
      if (((pre_q && (p->left == pre_p || p->right == pre_p)) ||
           (!p->left && !p->right)) &&
          ((pre_q && (q->left == pre_q || q->right == pre_q)) ||
           (!q->left && !q->right))) {
        if (p->val != q->val) return false;
        pre_p = p;
        pre_q = q;
        st_p.pop();
        st_q.pop();
      } else {
        if (p->right && q->right) {
          st_p.push(p->right);
          st_q.push(q->right);
        } else if (p->right || q->right) {
          return false;
        }
        if (p->left && q->left) {
          st_p.push(p->left);
          st_q.push(q->left);
        } else if (p->left || q->left) {
          return false;
        }
      }
    }
    return true;
  }
};

class TreeOrder {
 public:
  // 前序遍历
  void preOrder(TreeNode* root) {
    stack<TreeNode*> st;
    TreeNode* p = root;
    while (p || !st.empty()) {
      if (p) {
        cout << p->val << endl;
        p = p->left;
        st.push(p);
      } else {
        p = st.top();
        st.pop();
        p = p->right;
      }
    }
  }
  bool preOrder_issame(TreeNode* p, TreeNode* q) {
    stack<TreeNode*> st_p, st_q;
    TreeNode *p1 = p, *p2 = q;
    while (p1 || p2 || !st_p.empty() || !st_q.empty()) {
      if (p1 && p2) {
        if (p1->val != p2->val) return false;
        st_p.push(p1);
        st_q.push(p2);
        p1 = p1->left;
        p2 = p2->left;
      } else if (!p1 && !p2) {
        p1 = st_p.top();
        p2 = st_q.top();
        st_p.pop();
        st_q.pop();
        p1 = p1->right;
        p2 = p2->right;
      } else if (p1 || p2)
        return false;
    }
    return true;
  }
  // 中序列遍历
  void inOrder(TreeNode* root) {
    stack<TreeNode*> st;
    TreeNode* p = root;
    while (p || !st.empty()) {
      if (p) {
        st.push(p);
        p = p->left;
      } else {
        p = st.top();
        cout << p->val << endl;
        p = p->right;
      }
    }
  }

  bool inOrderSame(TreeNode* p, TreeNode* q) {
    stack<TreeNode*> st_p, st_q;
    TreeNode *p1 = p, *p2 = q;

    while (p1 || p2 || !st_p.empty() || !st_q.empty()) {
      if (p1 && p2) {
        st_p.push(p1);
        st_q.push(p2);
        p1 = p1->left;
        p2 = p2->left;
      } else if (!p1 && !p2) {
        p1 = st_p.top();
        p2 = st_q.top();
        st_p.pop();
        st_q.pop();
        if (p1->val != p2->val) return false;
        p1 = p1->right;
        p2 = p2->right;
      } else if (p1 || p2)
        return false;
    }
  }
  void outOrder(TreeNode* root) {
    stack<TreeNode*> st;
    TreeNode *p = root, *pre_p = NULL;
    while (p || !st.empty()) {
      p = st.top();
      if ((pre_p && (p->left == pre_p || p->right == pre_p)) ||
          (!p->left && !p->right)) {
        //如果当前结点没有孩子结点或者孩子节点都已被访问过
        cout << p->val << endl;
        pre_p = p;
        st.pop();
      } else {
        if (p->right) {
          st.push(p->right);
        }
        if (p->left) {
          st.push(p->left);
        }
      }
    }
  }
  bool outerSameTree(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;

    stack<TreeNode*> st_p, st_q;
    TreeNode *pre_p = NULL, *pre_q = NULL;
    st_p.push(p);
    st_q.push(q);
    while (!st_p.empty() || !st_q.empty()) {
      p = st_p.top();
      q = st_q.top();
      if (((pre_p && (p->left == pre_p || p->right == pre_p)) ||
           (!p->left && !p->right)) &&
          ((pre_q && (q->left == pre_q || q->right == pre_q)) ||
           (!q->left && !q->right))) {
        if (p->val != q->val) return false;
        pre_p = p;
        pre_q = q;
        st_p.pop();
        st_q.pop();
      } else {
        if (p->right && q->right) {
          st_p.push(p->right);
          st_q.push(q->right);
        } else if (p->right || q->right) {
          return false;
        }
        if (p->left && q->left) {
          st_p.push(p->left);
          st_q.push(q->left);
        } else if (p->left || q->left) {
          return false;
        }
      }
    }
    return true;
  }
};

enum Tag { VISIT = 0, PRINT = 1 };
struct Command {
  Tag tag;
  TreeNode* node;
  Command(Tag t, TreeNode* tn) : tag(t), node(tn) {}
};

// 前中后非递归遍历及递归遍历
class TreeOrderSimple {
 public:
  void preOrder(TreeNode* root) {
    stack<Command> st;
    st.push(Command(VISIT, root));
    while (!st.empty()) {
      Command cur = st.top();
      st.pop();
      if (cur.tag == PRINT) {
        cout << cur.node->val << endl;
      } else {
        if (cur.node->right) {
          st.push(Command(VISIT, cur.node->right));
        }
        if (cur.node->left) {
          st.push(Command(VISIT, cur.node->left));
        }
        st.push(Command(PRINT, cur.node));
      }
    }
  }
  bool preOrderSame(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;

    stack<Command> st_p, st_q;
    st_p.push(Command(VISIT, p));
    st_q.push(Command(VISIT, q));
    while (!st_p.empty() || !st_q.empty()) {
      Command cur_p = st_p.top();
      Command cur_q = st_q.top();
      st_p.pop();
      st_q.pop();
      if (cur_p.tag == PRINT && cur_q.tag == PRINT) {
        if (cur_p.node->val != cur_q.node->val) return false;
      } else if (cur_p.tag == VISIT && cur_q.tag == VISIT) {
        if (cur_p.node->right && cur_q.node->right) {
          st_p.push(Command(VISIT, cur_p.node->right));
          st_q.push(Command(VISIT, cur_q.node->right));
        } else if (cur_p.node->right || cur_q.node->right)
          return false;
        if (cur_p.node->left) {
          st_p.push(Command(VISIT, cur_p.node->left));
          st_q.push(Command(VISIT, cur_q.node->left));
        } else if (cur_p.node->left || cur_q.node->left)
          return false;
        st_p.push(Command(PRINT, cur_p.node));
        st_q.push(Command(PRINT, cur_q.node));
      } else
        return false;
    }
    return true;
  }
  void inOrder(TreeNode* root) {
    stack<Command> st;
    st.push(Command(VISIT, root));
    while (!st.empty()) {
      Command cur = st.top();
      st.pop();
      if (cur.tag == PRINT) {
        cout << cur.node->val << endl;
      } else {
        if (cur.node->right) {
          st.push(Command(VISIT, cur.node->right));
        }
        st.push(Command(PRINT, cur.node));
        if (cur.node->left) {
          st.push(Command(VISIT, cur.node->left));
        }
      }
    }
  }

  bool inOrderSame(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;

    stack<Command> st_p, st_q;
    st_p.push(Command(VISIT, p));
    st_q.push(Command(VISIT, q));
    while (!st_p.empty() || !st_q.empty()) {
      Command cur_p = st_p.top();
      Command cur_q = st_q.top();
      st_p.pop();
      st_q.pop();
      if (cur_p.tag == PRINT && cur_q.tag == PRINT) {
        if (cur_p.node->val != cur_q.node->val) return false;
      } else if (cur_p.tag == VISIT && cur_q.tag == VISIT) {
        if (cur_p.node->right && cur_q.node->right) {
          st_p.push(Command(VISIT, cur_p.node->right));
          st_q.push(Command(VISIT, cur_q.node->right));
        } else if (cur_p.node->right || cur_q.node->right)
          return false;
        st_p.push(Command(PRINT, cur_p.node));
        st_q.push(Command(PRINT, cur_q.node));
        if (cur_p.node->left) {
          st_p.push(Command(VISIT, cur_p.node->left));
          st_q.push(Command(VISIT, cur_q.node->left));
        } else if (cur_p.node->left || cur_q.node->left)
          return false;
      } else
        return false;
    }
    return true;
  }

  void outOrder(TreeNode* root) {
    stack<Command> st;
    st.push(Command(VISIT, root));
    while (!st.empty()) {
      Command cur = st.top();
      st.pop();
      if (cur.tag == PRINT) {
        cout << cur.node->val << endl;
      } else {
        st.push(Command(PRINT, cur.node));
        if (cur.node->right) {
          st.push(Command(VISIT, cur.node->right));
        }
        if (cur.node->left) {
          st.push(Command(VISIT, cur.node->left));
        }
      }
    }
  }
  bool outOrderSame(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;

    stack<Command> st_p, st_q;
    st_p.push(Command(VISIT, p));
    st_q.push(Command(VISIT, q));
    while (!st_p.empty() || !st_q.empty()) {
      Command cur_p = st_p.top();
      Command cur_q = st_q.top();
      st_p.pop();
      st_q.pop();
      if (cur_p.tag == PRINT && cur_q.tag == PRINT) {
        if (cur_p.node->val != cur_q.node->val) return false;
      } else if (cur_p.tag == VISIT && cur_q.tag == VISIT) {
        st_p.push(Command(PRINT, cur_p.node));
        st_q.push(Command(PRINT, cur_q.node));
        if (cur_p.node->right && cur_q.node->right) {
          st_p.push(Command(VISIT, cur_p.node->right));
          st_q.push(Command(VISIT, cur_q.node->right));
        } else if (cur_p.node->right || cur_q.node->right)
          return false;
        if (cur_p.node->left) {
          st_p.push(Command(VISIT, cur_p.node->left));
          st_q.push(Command(VISIT, cur_q.node->left));
        } else if (cur_p.node->left || cur_q.node->left)
          return false;
      } else
        return false;
    }
    return true;
  }
};

// 剑指 Offer 54. 二叉搜索树的第k大节点
// https://leetcode-cn.com/problems/er-cha-sou-suo-shu-de-di-kda-jie-dian-lcof/
class Solution {
 private:
  int res;

 public:
  int kthLargest(TreeNode* root, int k) {
    res = 0;
    dfs(root, k);
    return res;
  }

  void dfs(TreeNode* root, int& k) {
    if (!root) return;
    dfs(root->right, k);
    k--;
    if (k == 0) res = root->val;
    dfs(root->left, k);
  }
};

int kthLargest(TreeNode* root, int k) {
  TreeNode* p = root;
  stack<TreeNode*> st;
  while (p || !st.empty()) {
    if (p) {
      st.push(p);
      p = p->right;
    } else {
      p = st.top();
      st.pop();
      if (--k == 0) return p->val;
      p = p->left;
    }
  }
  return -1;
}

// 222. 完全二叉树的节点个数
// https://leetcode-cn.com/problems/count-complete-tree-nodes/
class Solution {
 public:
  int countNodes(TreeNode* root) {
    if (!root) return 0;
    int left = countNodes(root->left);
    int right = countNodes(root->right);
    return left + right + 1;
  }
};

class Solution {
 public:
  int countNodes(TreeNode* root) {
    if (!root) return 0;
    int leftH = leftHeight(root);
    int rightH = rightHeight(root);
    if (leftH == rightH) return pow(2, leftH) - 1;
    return 1 + countNodes(root->left) + countNodes(root->right);
  }

  int leftHeight(TreeNode* root) {
    if (!root) return 0;
    return 1 + leftHeight(root->left);
  }
  int rightHeight(TreeNode* root) {
    if (!root) return 0;
    return 1 + rightHeight(root->right);
  }
};

// 257. 二叉树的所有路径
// https://leetcode-cn.com/problems/binary-tree-paths/
class Solution {
 private:
  vector<string> ans;

 public:
  vector<string> binaryTreePaths(TreeNode* root) {
    ans = vector<string>();
    dfs(root, "");
    return ans;
  }
  void dfs(TreeNode* root, string path) {
    if (!root) return;

    path += to_string(root->val);
    if (root->left == NULL && root->right == NULL) {  // leaf node
      ans.push_back(path);
      return;
    }
    dfs(root->left, path + "->");
    dfs(root->right, path + "->");
  }
};

// 129. 求根到叶子节点数字之和
// https://leetcode-cn.com/problems/sum-root-to-leaf-numbers/
class Solution {
 private:
  int ans;

 public:
  int sumNumbers(TreeNode* root) {
    ans = 0;
    dfs(root, 0);
    return ans;
  }
  void dfs(TreeNode* root, int sum) {
    if (!root) return;

    sum = sum * 10 + root->val;
    if (root->left == NULL && root->right == NULL) {  // leaf node
      ans += sum;
      return;
    }
    dfs(root->left, sum);
    dfs(root->right, sum);
  }
  int sumNumbers(TreeNode* root) {
    int sum = 0;
    if (!root) return 0;
    queue<TreeNode*> tree_que;
    queue<int> num_que;
    tree_que.push(root);
    num_que.push(0);
    while (!tree_que.empty()) {
      TreeNode* cur = tree_que.front();
      int tmp = num_que.front() * 10 + cur->val;
      tree_que.pop();
      num_que.pop();
      if (cur->left == NULL && cur->right == NULL) {
        sum += tmp;
      }
      if (cur->left) {
        tree_que.push(cur->left);
        num_que.push(tmp);
      }
      if (cur->right) {
        tree_que.push(cur->right);
        num_que.push(tmp);
      }
    }
    return sum;
  }

  // 最小路径和
  int minPath(TreeNode* root) { return dfs(root); }
  int dfs(TreeNode* root) {
    if (!root) return 0;

    if (root->left == NULL && root->right == NULL) {  // leaf node
      return root->val;
    }
    int leftV = dfs(root->left);
    int rightV = dfs(root->right);
    return root->val + min(leftV, rightV);
  }
};

// 124. 二叉树中的最大路径和
// https://leetcode-cn.com/problems/binary-tree-maximum-path-sum/
class Solution {
 private:
  int maxV;

 public:
  // 最大路径和
  int maxPathSum(TreeNode* root) {
    maxV = INT_MIN;
    dfs(root);
    return maxV;
  }
  int dfs(TreeNode* root) {
    if (!root) return 0;
    int sum = root->val;
    int leftV = max(0, dfs(root->left));
    int rightV = max(0, dfs(root->right));
    maxV = max(maxV, root->val + leftV + rightV);
    return root->val + max(leftV, rightV);
  }
};

// 112.路径总和
// https://leetcode-cn.com/problems/path-sum/
class Solution {
 public:
  bool hasPathSum(TreeNode* root, int sum) { return dfs(root, sum); }
  bool dfs(TreeNode* root, int sum) {
    if (!root) return false;
    if (root->left == NULL && root->right == NULL) {
      if (sum == root->val) return true;
      return false;
    }
    bool left_res = dfs(root->left, sum - root->val);
    bool right_res = dfs(root->right, sum - root->val);
    return left_res || right_res;
  }
};

class Solution {
 public:
  bool hasPathSum(TreeNode* root, int sum) {
    if (!root) return false;
    stack<pair<TreeNode*, int>> st;
    st.push(make_pair(root, sum - root->val));
    while (!st.empty()) {
      auto cur = st.top();
      st.pop();
      if (!cur.first->left && !cur.first->right) {
        if (cur.second == 0) return true;
        continue;
      } else {
        if (cur.first->left) {
          st.push(
              make_pair(cur.first->left, cur.second - cur.first->left->val));
        }
        if (cur.first->right) {
          st.push(
              make_pair(cur.first->right, cur.second - cur.first->right->val));
        }
      }
    }
    return false;
  }

  bool hasPathSum(TreeNode* root, int sum) {
    if (!root) return false;
    stack<pair<TreeNode*, int>> st;
    st.push(make_pair(root, sum));
    while (!st.empty()) {
      auto cur = st.top();
      st.pop();
      if (!cur.first->left && !cur.first->right) {
        if (cur.second == cur.first->val) return true;
        continue;
      } else {
        if (cur.first->left) {
          st.push(make_pair(cur.first->left, cur.second - cur.first->val));
        }
        if (cur.first->right) {
          st.push(make_pair(cur.first->right, cur.second - cur.first->val));
        }
      }
    }
    return false;
  }
};

// 113.路径总和 II
// https://leetcode-cn.com/problems/path-sum-ii/
class Solution {
 private:
  vector<vector<int>> ans;

 public:
  vector<vector<int>> pathSum(TreeNode* root, int sum) {
    ans = vector<vector<int>>();
    vector<int> tmp;
    dfs(root, sum, tmp);
    return ans;
  }

  void dfs(TreeNode* root, int sum, vector<int> tmp) {
    if (!root) return;
    if (root->left == NULL && root->right == NULL) {
      if (root->val == sum) {
        tmp.push_back(root->val);
        ans.push_back(tmp);
      }
    }
    tmp.push_back(root->val);
    dfs(root->left, sum - root->val, tmp);
    dfs(root->right, sum - root->val, tmp);
  }
};

// 剑指 Offer 07. 重建二叉树
// https://leetcode-cn.com/problems/zhong-jian-er-cha-shu-lcof/
class Solution {
 public:
  // [1245367] [4251637]
  TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
    if (!preorder.size() || !inorder.size()) return NULL;

    int root_val = preorder[0];
    TreeNode* root = new TreeNode(root_val);
    int j = 0;
    for (; j < inorder.size(); j++) {
      if (inorder[j] == root_val) {
        break;
      }
    }
    vector<int> pre_left(preorder.begin() + 1, preorder.begin() + j + 1);
    // preorder [1...x] [x...n-1]
    vector<int> in_left(inorder.begin(), inorder.begin() + j);
    // inorder [0...j-1] [j+1,...n-1]
    root->left = buildTree(pre_left, in_left);

    vector<int> pre_right(preorder.begin() + j + 1, preorder.end());
    vector<int> in_right(inorder.begin() + j + 1, inorder.end());
    root->right = buildTree(pre_right, in_right);
    return root;
  }

 private:
  vector<int> preorder, inorder;

 public:
  TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
    if (!preorder.size() || !inorder.size()) return NULL;
    this->preorder = preorder;
    this->inorder = inorder;
    return dfs(0, preorder.size() - 1, 0, inorder.size() - 1);
  }
  TreeNode* dfs(int pl, int pr, int il, int ie) {
    if (pl > pr || il > ie) return NULL;
    int root_val = preorder[pl];
    TreeNode* root = new TreeNode(root_val);
    int i = 0;
    for (; i <= ie; i++) {
      if (inorder[i] == root_val) break;
    }

    root->left = dfs(pl + 1, pr - (ie - i), il, i - 1);
    root->right = dfs(pr - (ie - i) + 1, pr, i + 1, ie);
    return root;
  }

  TreeNode* buildTree(vector<int>& postorder, vector<int>& inorder) {
    if (!postorder.size() || !inorder.size()) return NULL;

    int root_val = postorder[postorder.size() - 1];
    TreeNode* root = new TreeNode(root_val);
    int j = 0;
    for (; j < inorder.size(); j++) {
      if (inorder[j] == root_val) {
        break;
      }
    }

    vector<int> post_left(postorder.begin(), postorder.begin() + j);
    // postorder [0...x] [x...n-2]
    vector<int> in_left(inorder.begin(), inorder.begin() + j);
    // inorder [0...j-1] [j+1,...n-1]
    root->left = buildTree(post_left, in_left);

    vector<int> post_right(postorder.begin() + j, postorder.end() - 1);
    vector<int> in_right(inorder.begin() + j + 1, inorder.end());
    root->right = buildTree(post_right, in_right);
    return root;
  }
};
