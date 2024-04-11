/*
 * @Author: gcxzhang
 * @Date: 2020-08-29 15:20:55
 * @LastEditTime: 2020-09-30 10:39:19
 * @Description: 200.岛屿的数量
 * https://leetcode-cn.com/problems/number-of-islands/
 * @FilePath: /alg/alg/bfs_dfs/number-of-islands.cpp
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
  int numIslands(vector<vector<char>> &grid) {
    n = grid.size();
    if (n == 0) {
      return 0;
    }
    m = grid[0].size();

    visited = vector<vector<bool>>(n, vector<bool>(m, false));
    int ans = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        if (grid[i][j] == '1' && !visited[i][j]) {
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
                  grid[newX][newY] == '1') {
                q.push({newX, newY});
                visited[newX][newY] = true;
              }
            }
          }
          ans++;
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
  int numIslands(vector<vector<char>> &grid) {
    n = grid.size();
    if (n == 0) {
      return 0;
    }
    m = grid[0].size();

    visited = vector<vector<bool>>(n, vector<bool>(m, false));

    int ans = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        if (grid[i][j] == '1' && !visited[i][j]) {
          int area = dfs(grid, i, j);
          ans++;
        }
      }
    }
    return ans;
  }

  int dfs(vector<vector<char>> &grid, int x, int y) {
    if (!inArea(x, y) || (inArea(x, y) && visited[x][y]) || grid[x][y] == '0')
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