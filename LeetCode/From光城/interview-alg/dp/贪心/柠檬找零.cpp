/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:52:09
 * @Description: 860. 柠檬水找零
 * https://leetcode-cn.com/problems/lemonade-change/
 * @FilePath: /alg/alg/dp/贪心/柠檬找零.cpp
 */

#include <vector>
using namespace std;
class Solution {
 public:
  bool lemonadeChange(vector<int>& bills) {
    int five = 0, ten = 0;
    for (auto& val : bills) {
      if (val == 5)
        five++;
      else if (val == 10) {
        if (five == 0) return false;
        five--;
        ten++;
      } else if (val == 20) {
        if (ten == 0) {
          if (five < 3) return false;
          five -= 3;
        } else if (five > 0) {
          ten -= 1;
          five -= 1;
        } else
          return false;
      }
    }
    return true;
  }
};