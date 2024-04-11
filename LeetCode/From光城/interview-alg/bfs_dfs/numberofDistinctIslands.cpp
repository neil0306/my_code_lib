/*
 * @Author: gcxzhang
 * @Date: 2020-08-29 16:29:24
 * @LastEditTime: 2020-09-30 08:38:39
 * @Description: 不同岛屿的数量
 * https://leetcode-cn.com/problems/number-of-distinct-islands/
 * @FilePath: /alg/bfs_dfs/numberofDistinctIslands.cpp
 */
/**
11
1
和
 1
11
是不同的岛屿，因为我们不考虑旋转和翻折。
 *
 */

#include <iostream>
#include <queue>
#include <set>
#include <vector>
using namespace std;
// 坐标相对位置 不记录基点(0,0),其余就是1,-1与0组合
class Solution {
 private:
  int direct[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  vector<vector<bool>> visited;
  int n, m;

 public:
  /**
   * @param grid: a list of lists of integers
   * @return: return an integer, denote the number of distinct islands
   */
  int numberofDistinctIslands(vector<vector<int>> &grid) {
    // write your code here
    n = grid.size();
    if (n == 0) {
      return 0;
    }
    m = grid[0].size();

    set<vector<pair<int, int>>> s;
    visited = vector<vector<bool>>(n, vector<bool>(m, false));
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        if (grid[i][j] == 1 && !visited[i][j]) {
          visited[i][j] = true;
          queue<pair<int, int>> q;
          q.push({i, j});
          vector<pair<int, int>> axis;
          while (!q.empty()) {
            int x = q.front().first;
            int y = q.front().second;
            q.pop();
            for (int k = 0; k < 4; k++) {
              int newX = x + direct[k][0];
              int newY = y + direct[k][1];
              if (inArea(newX, newY) && !visited[newX][newY] &&
                  grid[newX][newY] == 1) {
                q.push({newX, newY});
                visited[newX][newY] = true;
                axis.push_back({newX - i, newY - j});
              }
            }
          }
          s.insert(axis);
        }
      }
    }
    return s.size();
  }
  bool inArea(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }
};

class Solution {
 private:
  int direct[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  vector<vector<bool>> visited;
  int n, m;

 public:
  int numberofDistinctIslands(vector<vector<int>> &grid) {
    n = grid.size();
    if (n == 0) {
      return 0;
    }
    m = grid[0].size();

    visited = vector<vector<bool>>(n, vector<bool>(m, false));
    set<vector<pair<int, int>>> s;
    int ans = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        if (grid[i][j] == 1 && !visited[i][j]) {
          vector<pair<int, int>> axis;
          dfs(grid, i, j, axis);
          s.insert(axis);
        }
      }
    }
    return s.size();
  }

  void dfs(vector<vector<int>> &grid, int x, int y,
           vector<pair<int, int>> &axis) {
    if (!inArea(x, y) || (inArea(x, y) && visited[x][y]) || grid[x][y] == 0) {
      return;
    }
    visited[x][y] = true;
    for (int i = 0; i < 4; i++) {
      int newX = x + direct[i][0];
      int newY = y + direct[i][1];
      axis.push_back({newX - x, newY - y});
      dfs(grid, newX, newY, axis);
    }
  }
  bool inArea(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }
};

// 使用string 编码 去重
class Solution {
 private:
  int direct[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  vector<vector<bool>> visited;
  int n, m;

 public:
  /**
   * @param grid: a list of lists of integers
   * @return: return an integer, denote the number of distinct islands
   */
  int numberofDistinctIslands(vector<vector<int>> &grid) {
    // write your code here
    n = grid.size();
    if (n == 0) {
      return 0;
    }
    m = grid[0].size();

    set<string> s;
    visited = vector<vector<bool>>(n, vector<bool>(m, false));
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        if (grid[i][j] == 1 && !visited[i][j]) {
          visited[i][j] = true;
          queue<pair<int, int>> q;
          q.push({i, j});
          string encode = "";
          while (!q.empty()) {
            int x = q.front().first;
            int y = q.front().second;
            q.pop();
            for (int k = 0; k < 4; k++) {
              int newX = x + direct[k][0];
              int newY = y + direct[k][1];
              if (inArea(newX, newY) && !visited[newX][newY] &&
                  grid[newX][newY] == 1) {
                q.push({newX, newY});
                visited[newX][newY] = true;
                encode += to_string(newX - i) + "_" + to_string(newY - j) + "_";
              }
            }
          }
          s.insert(encode);
        }
      }
    }
    return s.size();
  }
  bool inArea(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }
};

class Solution {
 private:
  int direct[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  vector<vector<bool>> visited;
  int n, m;

 public:
  int numberofDistinctIslands(vector<vector<int>> &grid) {
    n = grid.size();
    if (n == 0) {
      return 0;
    }
    m = grid[0].size();

    visited = vector<vector<bool>>(n, vector<bool>(m, false));
    set<string> s;
    int ans = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        if (grid[i][j] == 1 && !visited[i][j]) {
          string encode = "";
          dfs(grid, i, j, encode);
          s.insert(encode);
        }
      }
    }
    return s.size();
  }

  void dfs(vector<vector<int>> &grid, int x, int y, string &encode) {
    if (!inArea(x, y) || (inArea(x, y) && visited[x][y]) || grid[x][y] == 0) {
      return;
    }
    visited[x][y] = true;
    for (int i = 0; i < 4; i++) {
      int newX = x + direct[i][0];
      int newY = y + direct[i][1];
      encode += to_string(newX - x) + "_" + to_string(newY - y) + "_";
      dfs(grid, newX, newY, encode);
    }
  }
  bool inArea(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }
};