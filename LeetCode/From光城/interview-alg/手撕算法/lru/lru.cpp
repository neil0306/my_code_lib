#include <iostream>
#include <unordered_map>
using namespace std;

class Node {
 public:
  int key;
  int val;
  Node* prev;
  Node* next;
  Node() { this->prev = this->next = nullptr; }
  Node(int key, int val) {
    this->key = key;
    this->val = val;
    this->prev = this->next = nullptr;
  }
};

class DoubleCycleLinkList {
 public:
  DoubleCycleLinkList() {
    head = new Node();
    tail = new Node();
    head->next = tail;
    tail->prev = head;
    node_size = 0;
  }
  void AddFirst(Node* node) { Insert(node, 0); }
  void AddBack(Node* node) {
    node->prev = tail->prev;
    node->next = tail;
    tail->prev->next = node;
    tail->prev = node;
    node_size++;
  }
  void Insert(Node* node, int pos) {
    if (pos != 0 && pos >= node_size) {
      AddBack(node);
      return;
    }
    Node* p = head;
    int i = 0;
    while (i != pos) {
      i++;
      p = p->next;
    }
    node->prev = p;
    node->next = p->next;
    p->next->prev = node;
    p->next = node;
    node_size++;
  }
  int GetSize() { return node_size; }
  void Remove(Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
  }

  Node* GetLast() { return tail->prev; }
  void TraverseList() {
    Node* p = head->next;
    while (p != tail) {
      cout << p->key << ":" << p->val << endl;
      p = p->next;
    }
  }
  ~DoubleCycleLinkList() {
    if (head && tail) {
      Node* p;
      while (head->next != tail) {  // 除去head的所有节点 包括tail
        p = head->next;
        head->next->next->prev = head;
        head->next = head->next->next;
        delete p;
      }
      delete head;
      delete tail;
    }
  }

 private:
  Node *head, *tail;
  int node_size;
};

class LRUCache {
 private:
  unordered_map<int, Node*> cache_;
  DoubleCycleLinkList double_cycle_link_list_;
  int cap_;

 public:
  LRUCache(int capacity) { cap_ = capacity; }
  int get(int key) {
    if (!cache_.count(key)) return -1;
    // 拿到这个数据同时，提升优先级
    put(key, cache_[key]->val);
    return cache_[key]->val;
  }
  void put(int key, int val) {
    if (cache_.count(key)) {
      Node* old_node = cache_[key];
      cache_.erase(key);
      double_cycle_link_list_.Remove(old_node);
    } else {
      if (cache_.size() == cap_) {
        Node* last = double_cycle_link_list_.GetLast();
        cache_.erase(last->key);
        double_cycle_link_list_.Remove(last);
      }
    }
    Node* new_node = new Node(key, val);
    double_cycle_link_list_.AddFirst(new_node);
    cache_[key] = new_node;
  }
};

int main() {
  DoubleCycleLinkList double_cycle_link_list;
  cout << "插入节点" << endl;
  Node* p = new Node(5, 10);
  double_cycle_link_list.AddBack(p);
  double_cycle_link_list.AddFirst(new Node(4, 11));
  double_cycle_link_list.AddFirst(new Node(3, 12));
  double_cycle_link_list.Insert(new Node(6, 11), 3);
  double_cycle_link_list.Insert(new Node(10, 11), 3);
  double_cycle_link_list.TraverseList();
  cout << "删除某个节点" << endl;
  double_cycle_link_list.Remove(p);
  double_cycle_link_list.TraverseList();
  cout << "获取最后一个节点" << endl;
  p = double_cycle_link_list.GetLast();
  cout << p->key << ":" << p->val << endl;
  cout << "删除某个节点" << endl;
  double_cycle_link_list.Remove(p);
  double_cycle_link_list.TraverseList();

  LRUCache* lru = new LRUCache(2);
  lru->put(2, 1);
  lru->put(2, 2);
  cout << lru->get(2) << endl;
  lru->put(1, 1);
  lru->put(4, 1);
  cout << lru->get(2) << endl;
  return 0;
}