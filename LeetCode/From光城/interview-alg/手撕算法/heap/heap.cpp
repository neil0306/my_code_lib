/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:17
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 08:14:37
 * @FilePath: /alg/heap/heap.cpp
 */
#include "heap.h"
class CmpMax {
 public:
  bool operator()(const int& a, int& b) { return (a > b); }
};

class CmpMin {
 public:
  bool operator()(const int& a, int& b) { return (a < b); }
};
void HeapTest(bool flag = true) {
  vector<int> data{5, 4, 6, 0, 10};
  using namespace lightcity;
  if (flag) {
    cout << "小根堆" << endl;
    Heap<int, CmpMin> heap(data);
    heap.GetData(data);
    for (const auto& elem : data) cout << elem << " ";
    cout << endl;
    cout << "push" << endl;
    heap.Push(-1);
    heap.GetData(data);
    for (const auto& elem : data) cout << elem << " ";
    cout << endl;
    vector<int> nums;
    heap.HeapSort(nums);
    for (const auto& elem : nums) cout << elem << " ";
    cout << endl;
  } else {
    cout << "大根堆" << endl;
    Heap<int, CmpMax> heap(data);
    heap.GetData(data);
    for (const auto& elem : data) cout << elem << " ";
    cout << endl;
    cout << "push" << endl;
    heap.Push(100);
    heap.GetData(data);
    for (const auto& elem : data) cout << elem << " ";
    cout << endl;
    vector<int> nums;
    heap.HeapSort(nums);
    for (const auto& elem : nums) cout << elem << " ";
    cout << endl;
  }
}
int main() {
  HeapTest();
  HeapTest(false);
}
