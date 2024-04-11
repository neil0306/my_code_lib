/*
 * @Author: gcxzhang
 * @Date: 2020-07-06 20:26:16
 * @LastEditTime: 2020-09-30 08:25:46
 * @Description: 手撕智能指针
 * @FilePath: /alg/sort/smart_ptr.cpp
 */
#include <iostream>
using namespace std;

template <typename T>
class scoped_ptr {
 private:
  T* ptr_;
  scoped_ptr(const scoped_ptr& other);
  scoped_ptr& operator=(const scoped_ptr& other);

 public:
  scoped_ptr(T* ptr = nullptr) noexcept : ptr_(ptr) {}
  T& operator*() { return *ptr_; }
  T* operator->() { return ptr_; }
  T* get() { return ptr_; }
  ~scoped_ptr() { delete ptr_; }
};

template <typename T>
class auto_ptr {
 private:
  T* ptr_;

 public:
  auto_ptr(T* ptr = nullptr) : ptr_(ptr) {}
  ~auto_ptr() { delete ptr_; }
  auto_ptr(auto_ptr& other) { ptr_ = other.release(); }
  T* release() {
    T* tmp = ptr_;
    ptr_ = nullptr;
    return tmp;
  }
  T& operator*() { return *ptr_; }
  T* operator->() { return ptr_; }
  auto_ptr operator=(auto_ptr& other) {
    auto_ptr tmp(other.release());
    swap(other);
  }
  void swap(auto_ptr& other) {
    using namespace std;
    swap(ptr_, other.ptr_);
  }
};

template <typename T>
class unique_ptr {
 private:
  T* ptr_;

 public:
  unique_ptr(T* ptr) noexcept : ptr_(ptr) {}
  ~unique_ptr() noexcept { delete ptr_; }
  T* release() {
    T* tmp = ptr_;
    ptr_ = nullptr;
    return tmp;
  }
  unique_ptr(unique_ptr&& other) noexcept { ptr_ = other.release(); }
  T& operator*() { return *ptr_; }
  T* operator->() { return ptr_; }
  unique_ptr operator=(unique_ptr other) noexcept {
    swap(other);
    return *this;
  }
  void swap(unique_ptr& other) noexcept {
    using namespace std;
    swap(ptr_, other_ptr_);
  }
};

// 引用计数
class shared_count {
 public:
  shared_count() : count_(1) {}
  ~shared_count() {}
  void add_count() { ++count_; }
  void reduce_count() { --count_; }
  long get_count() { return count_; }

 private:
  long count_;
};

template <typename T>
class shared_ptr {
 public:
  shared_ptr(T* ptr = nullptr) : ptr_(ptr) {
    if (ptr_) {
      shared_count_ = new shared_count();
    }
  }
  ~shared_count() {
    if (ptr_ && !shared_count_.reduce_count()) {
      delete ptr_;
      delete shared_count_;
    }
  }

  void swap(shared_ptr& other) {
    using namespace std;
    swap(ptr_, other.ptr_);
    swap(shared_count_, other.shared_count_);
  }
  shared_ptr(const shared_ptr& other) {
    ptr_ = other.ptr_;
    if (ptr_) {
      other.shared_count_.add_count();
      shared_count_ = other.shared_count_;
    }
  }
  shared_ptr(shared_ptr&& other) {
    ptr_ = other.ptr_;
    if (ptr_) {
      shared_count_ = other.shared_count_;
      other.shared_count_ = nullptr;
      other.ptr_ = nullptr;
    }
  }
  shared_ptr& operator=(shared_ptr other) {
    swap(other);
    return *this;
  }
  long use_count() {
    if (ptr_)
      return shared_count_->get_count();
    else
      return 0;
  }
  T& operator*() { return *ptr_; }
  T* operator->() { return ptr_; }

 private:
  T* ptr_;
  shared_count* shared_count_;
};