/*
 * @Author: gcxzhang
 * @Date: 2020-07-06 19:36:47
 * @LastEditTime: 2020-09-30 08:27:31
 * @Description: 手撕单例
 * @FilePath: /alg/sort/singleton.cpp
 */

#include <iostream>
namespace lightcity {

template <typename T>
class SingleHolder {
 private:
  SingleHolder();
  ~SingleHolder();
  SingleHolder(const SingleHolder& other);
  SingleHolder& operator=(const SingleHolder& other);
  static T* p;

 public:
  static T* Instance();
};

template <typename T>
T* SingleHolder<T>::p = nullptr;
template <typename T>
T* SingleHolder<T>::Instance() {
  T local_p;
  p = &local_p;
  return p;
}
};  // namespace lightcity

class A {
 public:
  int a1{100};
};
int main() {
  A* a = lightcity::SingleHolder<A>::Instance();
  std::cout << a->a1 << std::endl;
}
