/*
 * @Author: gcxzhang
 * @Date: 2020-08-29 18:30:11
 * @LastEditTime: 2020-09-30 08:39:01
 * @Description: 岛屿数量II
 * https://leetcode-cn.com/problems/number-of-islands-ii/
 * @FilePath: /alg/bfs_dfs/numDistinctIslands2.cpp
 */
/**
 * https://www.lintcode.com/problem/number-of-distinct-islands-ii/description
 */

#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <vector>
using namespace std;
class Solution {
 private:
  int direct[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  vector<vector<bool>> visited;
  int n, m;

 public:
  int numDistinctIslands2(vector<vector<int>>& grid) {
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
          vector<pair<int, int>> axis{{i, j}};
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
                axis.push_back({newX, newY});
              }
            }
          }

          s.insert(Normalize(axis));
        }
      }
    }
    return s.size();
  }
  vector<pair<int, int>> Normalize(const vector<pair<int, int>>& rawShape) {
    vector<vector<pair<int, int>>> shapes(8);  // 旋转+镜像

    // 得到每一种形状
    for (auto& sp : rawShape) {
      int x = sp.first;
      int y = sp.second;
      // 镜像
      shapes[0].push_back({x, y});
      shapes[1].push_back({x, -y});
      shapes[2].push_back({-x, y});
      shapes[3].push_back({-x, -y});
      // 旋转
      shapes[4].push_back({y, x});
      shapes[5].push_back({y, -x});
      shapes[6].push_back({-y, x});
      shapes[7].push_back({-y, -x});
    }

    for (auto& sp : shapes) {
      // 每种shape进行排序
      sort(sp.begin(), sp.end());
      // 得到这种shape的相对坐标
      for (int i = rawShape.size() - 1; i >= 0; i--) {
        sp[i].first -= sp[0].first;
        sp[i].second -= sp[0].second;
      }
    }

    // 所有相对坐标的不同shape进行排序
    sort(shapes.begin(), shapes.end());
    return shapes[0];  // 随便取一种
  }
  bool inArea(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }
};

class Solution {
 private:
  int direct[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  vector<vector<bool>> visited;
  int n, m;

 public:
  int numDistinctIslands2(vector<vector<int>>& grid) {
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
          s.insert(Normalize(axis));
        }
      }
    }
    return s.size();
  }

  void dfs(vector<vector<int>>& grid, int x, int y,
           vector<pair<int, int>>& axis) {
    if (!inArea(x, y) || (inArea(x, y) && visited[x][y]) || grid[x][y] == 0) {
      return;
    }
    visited[x][y] = true;
    for (int i = 0; i < 4; i++) {
      int newX = x + direct[i][0];
      int newY = y + direct[i][1];
      axis.push_back({newX, newY});
      dfs(grid, newX, newY, axis);
    }
  }
  vector<pair<int, int>> Normalize(const vector<pair<int, int>>& rawShape) {
    vector<vector<pair<int, int>>> shapes(8);  // 旋转+镜像

    // 得到每一种形状
    for (auto& sp : rawShape) {
      int x = sp.first;
      int y = sp.second;
      // 镜像
      shapes[0].push_back({x, y});
      shapes[1].push_back({x, -y});
      shapes[2].push_back({-x, y});
      shapes[3].push_back({-x, -y});
      // 旋转
      shapes[4].push_back({y, x});
      shapes[5].push_back({y, -x});
      shapes[6].push_back({-y, x});
      shapes[7].push_back({-y, -x});
    }

    for (auto& sp : shapes) {
      // 每种shape进行排序
      sort(sp.begin(), sp.end());
      // 得到这种shape的相对坐标
      for (int i = rawShape.size() - 1; i >= 0; i--) {
        sp[i].first -= sp[0].first;
        sp[i].second -= sp[0].second;
      }
    }

    // 所有相对坐标的不同shape进行排序
    sort(shapes.begin(), shapes.end());
    return shapes[0];  // 随便取一种
  }
  bool inArea(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }
};