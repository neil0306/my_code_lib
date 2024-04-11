/*
 * @Author: gcxzhang
 * @Date: 2020-08-28 15:17:59
 * @LastEditTime: 2020-09-30 10:38:48
 * @Description: 695.岛屿的最大面积
 * https://leetcode-cn.com/problems/max-area-of-island/
 * @FilePath: /alg/alg/bfs_dfs/max-area-of-island.cpp
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
  int maxAreaOfIsland(vector<vector<int>> &grid) {
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
          int area = 1;
          while (!q.empty()) {
            int x = q.front().first;
            int y = q.front().second;
            q.pop();
            for (int k = 0; k < 4; k++) {
              int newX = x + direct[k][0];
              int newY = y + direct[k][1];
              if (inArea(newX, newY) && !visited[newX][newY] &&
                  grid[newX][newY] == 1) {
                area++;
                q.push({newX, newY});
                visited[newX][newY] = true;
              }
            }
          }
          ans = max(ans, area);
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
  int maxAreaOfIsland(vector<vector<int>> &grid) {
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
          ans = max(ans, area);
        }
      }
    }
    return ans;
  }

  int dfs(vector<vector<int>> &grid, int x, int y) {
    if (!inArea(x, y) || (inArea(x, y) && visited[x][y]) || grid[x][y] == 0)
      return 0;
    int area = 1;  // (x,y)
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