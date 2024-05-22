# try_ lock

Few points to remember about the try_lock is as follows:
0. `try_lock()` Tries to lock the mutex. **Returns immediately**. On **successful lock acquisition returns true** otherwise returns false.
1. If try_lock() is not able to lock mutex, then it doesn't get blocked that's why it is called non-blocking 
    - 如果try_lock没有成功, 则当前这个函数就相当于啥事没干, 直接返回一个false.
2. If try_lock is called again by the same thread which owns the mutex, the behaviour is undefined.
   It is a **dead lock situation** with undefined behaviour. 
   - if you want to be able to lock the same mutex by same thread more than one time the go for `recursive_mutex`.
     - 如果对同一个线程使用两次 try_lock, 就会触发`未定义行为`, 甚至发生`死锁`.
     - 如果真的就想要对同一个线程实现两次 lock, 那应该使用 recursive_mutex 函数.


There are so many try_lock function
1. std::try_lock
2. **std::mutex::try_lock**      <==== 这里主要讲这个函数
3. std::shared_lock::try_lock
4. std::timed_mutex::try_lock
5. std::unique_lock::try_lock
6. std::shared_mutex::try_lock
7. std::recursive_mutex::try_lock
8. std::shared_timed_mutex::try_lock
9. std::recursive_timed_mutex::try_lock

