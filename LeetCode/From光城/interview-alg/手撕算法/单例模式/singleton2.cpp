/*
 * @Author: gcxzhang
 * @Date: 2020-07-06 20:06:49
 * @LastEditTime: 2020-09-30 11:10:34
 * @Description: 手撕单例模式
 * @FilePath: /alg/alg/sort/singleton2.cpp
 */
#include <iostream>
#include <mutex>
namespace {
template <typename T>
class SingletonHolder {
 private:
  SingletonHolder();
  ~SingletonHolder();
  SingletonHolder(const SingletonHolder& other);
  SingletonHolder& operator=(const SingletonHolder& other);
  static T* p;
  static std::mutex lock_;

 public:
  static T* Instance();
};
template <typename T>
T* SingletonHolder<T>::p = nullptr;

template <typename T>
T* SingletonHolder<T>::Instance() {
  if (!p) {
    lock_guard<mutex> lg(lock_);
    if (!p) {
      T* tmp = (T*)operator new(sizeof(T));
      new (p) T();
      p = tmp;
    }
  }
  return p;
}
};  // namespace
