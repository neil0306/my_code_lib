/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:16
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:41:06
 * @Description: 773. 滑动谜题
 * https://leetcode-cn.com/problems/sliding-puzzle/
 * @FilePath: /alg/alg/bfs_dfs/slidingPuzzle.cpp
 */
#include <climits>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>
using namespace std;
class Solution {
 private:
  int d[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  int n, m;

 public:
  int slidingPuzzle(vector<vector<int>>& board) {
    n = board.size();
    m = board[0].size();
    string start = boardToString(board);
    int step = 0;
    unordered_set<string> s;
    queue<string> q;
    q.push(start);
    s.insert(start);
    while (!q.empty()) {
      int sz = q.size();
      for (int i = 0; i < sz; i++) {
        string cur = q.front();
        q.pop();
        if (cur == "123450") return step;
        int index = cur.find('0');
        int x = index / m;
        int y = index % m;
        cout << x << " " << y << endl;
        for (int j = 0; j < 4; j++) {
          int x1 = x + d[j][0];
          int y1 = y + d[j][1];
          if (inBoard(x1, y1)) {
            string tmp = cur;
            cout << tmp[index] << " " << tmp[x1 * m + y1] << endl;
            swap(tmp[index], tmp[x1 * m + y1]);
            if (!s.count(tmp)) {
              q.push(tmp);
              s.insert(tmp);
            }
          }
        }
      }
      step++;
    }

    return -1;
  }
  bool inBoard(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }
  string boardToString(const vector<vector<int>>& board) {
    string res = "";
    for (auto elem : board)
      for (auto e : elem) res += to_string(e);
    return res;
  }
};
class Solution1 {
 private:
  int d[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  int n, m;
  unordered_set<string> visited;
  int ret;

 public:
  int slidingPuzzle(vector<vector<int>>& board) {
    n = board.size();
    m = board[0].size();
    ret = INT_MAX;
    string start = boardToString(board);
    visited = unordered_set<string>();
    dfs(start, 0);
    return ret == INT_MAX ? -1 : ret;
  }

  void dfs(string s, int ans) {
    if (s == "123450") {
      ret = min(ret, ans);
      return;
    }

    visited.insert(s);
    int index = s.find('0');
    int x = index / m;
    int y = index % m;

    for (int j = 0; j < 4; j++) {
      int x1 = x + d[j][0];
      int y1 = y + d[j][1];
      if (inBoard(x1, y1)) {
        swap(s[index], s[x1 * m + y1]);
        if (!visited.count(s)) dfs(s, ans + 1);
        swap(s[x1 * m + y1], s[index]);
      }
    }
    visited.erase(s);
    return;
  }
  bool inBoard(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }
  string boardToString(const vector<vector<int>>& board) {
    string res = "";
    for (auto elem : board)
      for (auto e : elem) res += to_string(e);
    return res;
  }
};
int main() {
  vector<vector<int>> board = {{1, 2, 3}, {5, 4, 0}};
  cout << Solution1().slidingPuzzle(board);
  return 0;
}
