#include <cassert>
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;
namespace lightcity {
template <typename T, typename Cmp = std::less<T>>
class Heap {
 private:
  vector<T> data_;
  Cmp cmp;

 public:
  Heap() {}
  Heap(T capacity) { data_ = vector<T>(capacity); }
  // heapify
  // 从最后一个非叶子结点开始向下siftdown 直到扫描完所有非叶子结点
  Heap(vector<T>& heap) {
    data_ = heap;
    T last_not_leaf = GetParent(data_.size() - 1);
    while (last_not_leaf >= 0) {
      SiftDown(last_not_leaf);
      last_not_leaf--;
    }
  }
  void SiftDown(int k) {
    while (GetLeftChild(k) < data_.size()) {
      int j = GetLeftChild(k);
      if (j + 1 < data_.size() && cmp(data_[j + 1], data_[j]))
        j = GetRightChild(k);  // j++
      // data[j]是此时leftChild 和 rightChild中的最大值
      if (cmp(data_[k], data_[j])) break;
      swap(k, j);
      k = j;
    }
  }
  void SiftUp(int k) {
    while (k > 0 && cmp(data_[k], data_[GetParent(k)])) {
      int p = GetParent(k);
      swap(p, k);
      k = p;
    }
  }
  void HeapSort(vector<int>& nums) {
    while (data_.size() > 0) {
      nums.push_back(Top());
      Pop();
    }
  }

  void Push(T value) {
    data_.push_back(value);
    SiftUp(data_.size() - 1);
  }

  void Pop() {
    swap(0, data_.size() - 1);
    data_.pop_back();
    SiftDown(0);
  }

  void swap(int i, int j) {
    T t = data_[i];
    data_[i] = data_[j];
    data_[j] = t;
  }
  int GetParent(int child) {
    assert(child > 0);
    return (child - 1) / 2;
  }
  int GetLeftChild(int parent) { return 2 * parent + 1; }
  int GetRightChild(int parent) { return 2 * parent + 2; }

  void GetData(vector<T>& data) { data = data_; }
  int GetSize() { return data_.size(); }
  T Top() {
    assert(data_.size() > 0);
    return data_[0];
  }
  bool Empty() { return data_.empty(); }
};

template <typename T, typename Cmp = std::less<T>>
class priority_queue {
 private:
  Heap<T, Cmp> heap;

 public:
  priority_queue() {}
  int GetSize() { return heap.GetSize(); }

  void Push(T value) { heap.Push(value); }

  T Top() { return heap.Top(); }

  void Pop() { heap.Pop(); }

  bool Empty() { return heap.Empty(); }
};
};  // namespace lightcity
