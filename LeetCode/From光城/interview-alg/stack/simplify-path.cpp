/*
 * @Author: 光城
 * @Date: 2020-09-30 10:06:09
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 11:10:12
 * @Description: 71.简化路径 https://leetcode-cn.com/problems/simplify-path/
 * @FilePath: /alg/alg/stack/simplify-path.cpp
 */
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>
using namespace std;
class Solution {
 private:
 public:
  vector<string> paths;
  void split(string path) {
    int start = 0;
    int end;
    while ((end = path.find("/", start)) != string::npos) {
      paths.push_back(string(path.begin() + start, path.begin() + end));
      start = end + 1;
    }
    paths.push_back(string(path.begin() + start, path.end()));
  }

  string simplifyPath(string path) {
    split(path);
    cout << endl;
    map<string, int> notDirs{{"", 0}, {".", 0}, {"..", 0}};
    stack<string> st;
    for (auto& p : paths) {
      if (notDirs.count(p) == 0) {
        st.push(p);
      } else if (p == ".." && !st.empty()) {
        st.pop();
      }
    }
    if (st.empty()) return "/";
    string ans;
    while (!st.empty()) {
      ans = "/" + st.top() + ans;
      st.pop();
    }
    return ans;
  }

  string simplifyPath(string path) {
    stack<string> st;
    for (int i = 0; i < path.size(); i++) {
      // 去掉多余的/
      while (i < path.size() && path[i] == '/') i++;
      if (i == path.size()) break;
      string tmp;
      while (i < path.size() && path[i] != '/') tmp += path[i++];
      if (tmp == "..") {
        if (!st.empty()) st.pop();
      } else if (tmp != "" && tmp != ".") {
        st.push(tmp);  // 是在不等于..情况下
      }
    }
    string ans;
    if (st.empty()) return "/";
    while (!st.empty()) {
      ans = "/" + st.top() + ans;
      st.pop();
    }
    return ans;
  }
};

int main() {
  string path = "/home/./fs/";
  Solution s;
  cout << s.simplifyPath(path) << endl;
  return 0;
}