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
class Solution
{
	public:
		// 递归版本
		bool compare(TreeNode* left, TreeNode* right)
		{
			// stop condition
			if(!left && !right) {return true;}		// 都是空子树，返回true
			if((!left && right) || (left && !right)){return false;}
			if(left->val != right-> val){return false;}

			bool left = compare(left->left, right->left);
			bool right = compare(left->right, right->right);
			return left && right;
		}	

    	bool isSameTree(TreeNode* p, TreeNode* q) 
	    {
	        bool res = compare(p,q);
	        return res;
	    }



		// 队列版本
		bool isSameTree(TreeNode* p, TreeNode* q)
		{
			if (!p && !q){return true;}
			queue<TreeNode*> que;
			que.push(p);
			que.push(q);

			while(!que.empty()){
				TreeNode* left = que.front();
				que.pop();
				TreeNode* right = que.front()l
				que.pop();

				if(!left && !right){
					continue;
				} 
				else if ((!left && right) || (left && !right)){
					return false;
				}
				else if (left->val != right->val){
					return false;
				}

				que.push(left->left);
				que.push(right->left);
				que.push(left->right);
				que.push(right->right);
			} 
			return true;
		}


		// 栈版本
		bool isSameTree(TreeNode * p, TreeNode * q)
		{
			if(!p && !q){return true;}
			stack<TreeNode*> st;
			st.push(p);
			st.push(q);

			while(!st.empty()){
				TreeNode * left = st.top();
				st.pop();
				TreeNode * right = st.top();
				st.pop();

				if(!left && !right){
					continue;
				}
				if((!left && right) || (left && !right)){
					return false;
				}
				if(left->val != right->val){
					return false;
				}

				st.push(left->right);   // 栈：后进先出，故先进right，再进left
				st.push(right->right);
				st.push(left->left);
				st.push(right->left);
			}

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
    int res, k;   // 一定要用全局变量来维护这个k
    void dfs(TreeNode * cur)
    {
        if(!cur){return;}
        if(k==0){return;} // 在这里需要提前返回，也就是当 k == 0 时提前返回，否则会继续后面的遍历

        dfs(cur->right);
        if(--k == 0){
            res = cur->val;
        }
        dfs(cur->left);
    }
public:
    int findTargetNode(TreeNode* root, int cnt) {
        k = cnt;
        dfs(root);
        return res;
    }
};

int findTargetNode(TreeNode* root, int cnt) {
    TreeNode* p = root;
    stack<TreeNode*> st;
    while (p || !st.empty()) {
        if (p) {
        st.push(p);
        p = p->right;
        } else {
        p = st.top();
        st.pop();
        if (--cnt == 0) return p->val;
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
  // 递归
  int sumNumbers(TreeNode* root) {
    ans = 0;
    dfs(root, 0);
    return ans;
  }
  void dfs(TreeNode* root, int sum) {
    if (!root) return;

    sum = sum * 10 + root->val;    // 乘以10倍，腾了位子给新元素； 
                                   // 由于每次进入递归都会自动拷贝一次 sum，所以相当于回溯到上一层的时候sum是使用当时那层的旧数值
    if (root->left == NULL && root->right == NULL) {  // leaf node
      ans += sum;   // 每当走到叶子节点，就累加
      return;
    }
    dfs(root->left, sum);   // 如果root是叶子节点，往下走之后就会马上返回。
    dfs(root->right, sum);
  }

  // 迭代法
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

    if (root->left == NULL && root->right == NULL) {  // 【中】 leaf node 
      return root->val;
    }
    int leftV = dfs(root->left);    // 【左】
    int rightV = dfs(root->right);  // 【右】
    return root->val + min(leftV, rightV);  // 当前节点 + 左右子树最小路径和
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
    int leftV = max(0, dfs(root->left));  // 【左】注意是与0比较大小，也就是相当于把 root->left 当作一颗独立的二叉树进行处理
    int rightV = max(0, dfs(root->right)); // 【右】同上，将root->right 当作一颗独立二叉树处理了
    maxV = max(maxV, root->val + leftV + rightV);  // 【中】将当前节点考虑进来，看看这个路径是否与历史路径相比以前的大
    return root->val + max(leftV, rightV); // 注意返回的时候包含当前节点
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
    bool left_res = dfs(root->left, sum - root->val);   // sum - root->val 这里其实隐藏了一个回溯的过程（因为变量是按值传递），进入递归是sum变小了，出递归的时候sum恢复原来的值
    bool right_res = dfs(root->right, sum - root->val);
    return left_res || right_res;
  }
};

class Solution {
 public:
  bool hasPathSum(TreeNode* root, int sum) {
    if (!root) return false;
    stack<pair<TreeNode*, int>> st;
    st.push(make_pair(root, sum - root->val));   // 这里减掉了 root->val
    while (!st.empty()) {
      auto cur = st.top();
      st.pop();
      if (!cur.first->left && !cur.first->right) {    // 递归法里的“递归停止条件”
        if (cur.second == 0) return true;
        continue;     // 注意是continue
      } else {
        if (cur.first->left) {
          st.push(
              make_pair(cur.first->left, cur.second - cur.first->left->val));  // sum的减小发生在第二个参数位置，由于最开始入栈的时候已经减掉cur的val，所以这里是孩子节点的val
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
    st.push(make_pair(root, sum));    // 直接将sum入栈
    while (!st.empty()) {
      auto cur = st.top();
      st.pop();
      if (!cur.first->left && !cur.first->right) {
        if (cur.second == cur.first->val) return true;
        continue;
      } else {
        if (cur.first->left) {
          st.push(make_pair(cur.first->left, cur.second - cur.first->val)); // 这里直接用的 cur.first->val
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

  void dfs(TreeNode* root, int sum, vector<int> tmp) { // 这里的tmp存储的是走到当前节点之前的路径；注意sum和tmp都不是按引用传递，是按值传递
    if (!root) return;
    if (root->left == NULL && root->right == NULL) { // 叶子节点
      if (root->val == sum) { // 满足条件
        tmp.push_back(root->val);
        ans.push_back(tmp);
      }
    }
    tmp.push_back(root->val);
    dfs(root->left, sum - root->val, tmp);  // 注意这里的 (sum - root->val)，一定要写在传参的位置，否则回溯会出问题
    dfs(root->right, sum - root->val, tmp);
  }
};

// 剑指 Offer 07. 重建二叉树
// https://leetcode-cn.com/problems/zhong-jian-er-cha-shu-lcof/
// https://leetcode.cn/problems/construct-binary-tree-from-preorder-and-inorder-traversal
  // 核心思想：前序遍历确定中间节点，中序遍历找出左、右节点
class Solution {
 public:
  // [1245367] [4251637]
  TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
    if (!preorder.size() || !inorder.size()) return NULL;  // 因为是遍历同一棵树，所以只要有一个数组节点数量不同，肯定无法重建

    int root_val = preorder[0];   // 中序遍历的第一个节点一定是根节点（中间节点）
    TreeNode* root = new TreeNode(root_val);
    int j = 0;
    for (; j < inorder.size(); j++) {
      if (inorder[j] == root_val) { // 在中序遍历中找出当前子树的根节点
        break;
      }
    }
    vector<int> pre_left(preorder.begin() + 1, preorder.begin() + j + 1); // 注意区间为左闭右开
    // preorder [1...x] [x...n-1]
    vector<int> in_left(inorder.begin(), inorder.begin() + j);            // 左闭右开
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
