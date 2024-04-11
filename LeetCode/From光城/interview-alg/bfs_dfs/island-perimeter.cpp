/*
 * @Author: gcxzhang
 * @Date: 2020-08-29 15:41:46
 * @LastEditTime: 2020-09-30 08:36:35
 * @Description: 463.岛屿的周长
 * https://leetcode-cn.com/problems/island-perimeter/
 * @FilePath: /alg/bfs_dfs/island-perimeter.cpp
 */
#include <iostream>
#include <queue>
#include <vector>
using namespace std;
class Solution {
 private:
  int direct[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  vector<vector<bool>> visited;
  int n, m;

 public:
  // bfs
  int islandPerimeter(vector<vector<int>> &grid) {
    n = grid.size();
    if (n == 0) {
      return 0;
    }
    m = grid[0].size();

    visited = vector<vector<bool>>(n, vector<bool>(m, false));
    int ans = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        if (grid[i][j] == 1 && !visited[i][j]) {
          visited[i][j] = true;
          queue<pair<int, int>> q;
          q.push({i, j});
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
              } else if (!inArea(newX, newY) ||
                         grid[newX][newY] ==
                             0)  // 陆地变水域或边界 加1 水域之间不变
              {
                ans++;
              }
            }
          }
        }
      }
    }
    return ans;
  }
  bool inArea(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }
};

class Solution {
 private:
  int direct[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  vector<vector<bool>> visited;
  int n, m;

 public:
  // dfs
  int islandPerimeter(vector<vector<int>> &grid) {
    n = grid.size();
    if (n == 0) {
      return 0;
    }
    m = grid[0].size();

    visited = vector<vector<bool>>(n, vector<bool>(m, false));

    int ans = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        if (grid[i][j] == 1 && !visited[i][j]) {
          int area = dfs(grid, i, j);
          ans += area;
        }
      }
    }
    return ans;
  }

  int dfs(vector<vector<int>> &grid, int x, int y) {
    // 陆地变水域或边界 加1 水域之间不变
    if (!inArea(x, y) || grid[x][y] == 0) return 1;
    if (inArea(x, y) && visited[x][y]) return 0;
    int area = 0;
    visited[x][y] = true;
    for (int i = 0; i < 4; i++) {
      int newX = x + direct[i][0];
      int newY = y + direct[i][1];
      area += dfs(grid, newX, newY);
    }
    return area;
  }
  bool inArea(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }
};

class Solution {
 public:
  // 由于岛屿内没有湖,所以只需要求出 北面(或南面) + 西面(或东面)的长度再乘2即可
  int islandPerimeter(vector<vector<int>> &grid) {
    int n = grid.size();
    if (n == 0) return 0;
    int m = grid[0].size();
    int ans = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        if (grid[i][j] == 1) {
          if (i == 0 || grid[i - 1][j] == 0) ans++;
          if (j == 0 || grid[i][j - 1] == 0) ans++;
        }
      }
    }
    return ans * 2;
  }
};