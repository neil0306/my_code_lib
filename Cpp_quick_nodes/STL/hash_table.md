# 哈希表 (HashTable)

哈希表是STL中非常重要的底层结构, 诸如 `unordered_set`, `unordered_map`等无序关联容器的内部都是基于哈希表实现的.

这里主要用一个简单的哈希表实现来讲述`哈希表`, `哈希冲突`, `迭代器`, `性能优化`等方面的问题.

## 哈希表原理分析
1. 哈希表基础知识
    > 哈希表是一种`通过哈希函数将 键(key) 映射为 索引(index) 的数据结构`. 
    
    > 哈希函数负责将任意大小的输入映射到固定大小的输出, 即哈希值. 哈希值用作数组中关联 key-value 的索引.

2. 哈希冲突
    > 由于哈希函数的映射不是一对一的, 可能会`存在两个不同的key映射为同一个index的情况`, 这就是哈希冲突.

    > 哈希冲突可以使用`链地址法`来解决, 即在哈希表的每一个 bucket 中维护一个链表, 将哈希值相同的元素存储在同一个 bucket 的链表中.

3. 哈希表的扩容与 **rehashing**
   > 为了避免哈希表中链表过长导致的性能下降, 通常会在需要时对哈希表进行扩容.

   > 扩容的过程涉及`重新计算所有元素的哈希值`, 并将他们分布到新的更大的哈希表中. 这个过程就称为**rehashing**.

4. 性能优化
   > 为了提高性能, 哈希表的实现通常包含一系列优化策略, 比如: 使用`二次哈希函数`, `空间配置器`, `内存池`等等, 以减少内存分配的开销, 提高访问速度.

5. 并发性与多线程安全性
    > 在多线程环境中, 确保数据结构的正确性至关重要.
    
    > 多线程哈希表的视线通常需要考虑并发性和多线程安全性, 通过`锁`或者其他机制来保护共享的数据结构.

    - 下图是一个哈希表的插入元素的例子:
    ![](hash_table_images/哈希表中插入元素的过程.png)
      - 上述过程中, 假设每个bucket链表长度大于1就开始rehash
      - 可以看到, 每个bucket用一个链表来存储重复的元素, 如8和17


# HashTable 的简单实现

这里的简单版本没有STL中的那么复杂和高效, 只包含很常用的基本功能, 包括`插入`, `删除`, `查找`, `打印`, `集合大小`等.


```cpp
#include <iostream>
#include <algorithm>
#include <functional>
#include <cstddef>
#include <list>     // 解决哈希冲突用到链表
#include <utility>
#include <vector>
#include <sstream>
#include <string>

// 三个泛型: Key, Value, Hash
template <typename Key, typename Value, typename Hash = std::hash<Key>>  
class HashTable
{
private:
    class HashNode
    {
    public:
        Key key;
        Value value;

        // 从Key构造节点, Value使用默认构造
        explicit HashNode(const Key &key) : key(key), value() {}

        // 从Key和Value构造节点
        HashNode(const Key &key, const Value &value) : key(key), value(value) {}

        // 比较运算符重载, 只按照key进行比较
        bool operator==(const HashNode &other) const { return key == other.key; }

        bool operator!=(const HashNode &other) const { return key != other.key; }

        bool operator<(const HashNode &other) const { return key < other.key; }

        bool operator>(const HashNode &other) const { return key > other.key; }
        
        bool operator==(const Key &key_) const { return key == key_; }

        void print() const
        {
            std::cout << key << " " << value << " ";
        }
    };

private:
    using Bucket = std::list<HashNode>;  // 指定桶的类型为 "存储key的链表"
    std::vector<Bucket> buckets;         // 存储所有桶的动态数组
    Hash hashFunction;                   // 哈希函数对象 (这里用的是标准库自带的哈希函数)
    size_t tableSize;                    // 哈希表大小 (桶的大小)
    size_t numElements;                   // 哈希表中元素的数量 (key-value pair的数量)

    float maxLoadFactor = 0.75;                 // 默认的最大负载因子
    
    // 计算key的哈希值, 并将其映射到桶的索引
    size_t hash(const Key &key) const { return hashFunction(key) % tableSize; }   // 先用哈希函数计算一个映射值, 然后用tableSize换算到对应的bucket

    // 当元素数量超过最大负载因子定义的容量时, 增加桶的数量并重新分配所有的key
    void rehash(size_t newSize)
    {
        std::vector<Bucket> newBucket(newSize);     // 创建新的桶数组

        // 搬移旧数据
        for (Bucket & bucket : buckets){            // 遍历旧桶
            for (HashNode &hashNode : bucket){      // 遍历桶里的每一个 key
                size_t newIndex = hashFunction(hashNode.key) % newSize;
                newBucket[newIndex].push_back(hashNode);
            }
        }
        buckets = std::move(newBucket);            // 移动语义, 更新桶数组
        tableSize = newSize;                        // 更新大小
    }

public:
    // 构造函数初始化哈希表
    HashTable(size_t size = 10, const Hash &hashFunc = Hash()) 
                : buckets(size), hashFunction(hashFunc), tableSize(size), numElements(0) {}

    // 插入新的 key-value 到哈希表
    void insert(const Key &key, const Value &value)
    {
        if((numElements + 1) > maxLoadFactor * tableSize){  // 判断是否需要rehash
            // 处理 clear 后再次插入元素时 tableSize 为 0 的场景, 否则下面的rehash可能并没有扩容
            if(tableSize == 0) tableSize = 1;
        
            rehash(tableSize * 2);                  // rehash, 桶的数量翻倍 
        }

        size_t index = hash(key);                       // 计算新key的index
        std::list<HashNode> &bucket = buckets[index];   // 获取对应的桶 (桶在这里就是指一个链表, 获取的是链表头)

        // 如果key不在桶里, 则添加到桶中
        if (std::find(bucket.begin(), bucket.end(), key) == bucket.end()){  // 找到迭代器末尾都没找到key
            bucket.push_back(HashNode(key, value));  
            ++numElements;
        }
    }

    void insertKey(const Key &key) { insert(key, Value{}); }    // value是空的, 用Value的默认构造初始化了一个值塞进去

    // 删除哈希表中指定的key
    void erase(const Key &key)
    {
        size_t index = hash(key);
        auto &bucket = buckets[index];  // 获取桶的引用, 注意是引用类型.
        auto it = std::find(bucket.begin(), bucket.end(), key);  // 在指定的桶里查找key的位置
        if(it != bucket.end()){         // 找到key, 进行删除操作
            bucket.erase(it);           // bucket本质上是一个vector, 删除的时候用erase比较安全
            numElements--;
        }
    }

    // 查找键是否存在于哈希表中, 找到的话就返回 Value 的指针, 没找到返回 nullptr
    Value *find(const Key &key)
    {
        size_t index = hash(key);
        auto &bucket = buckets[index];  // 获取桶的引用, 注意是引用类型.
        auto ans = std::find(bucket.begin(), bucket.end(), key);  // 在指定的桶里查找key的位置, 
        if(ans != bucket.end()){
            return &ans->value;         // ans 的具体类型是一个迭代器, 并且是 HashNode 类型
        }
        return nullptr;
    }

    size_t size() const { return numElements; }

    // 打印哈希表中的所有元素
    void print() const
    {
        for (size_t i = 0; i < buckets.size(); ++i){        // 遍历所有的桶 (每一个桶里都是链表, 链表的节点是 HashNode 类型)
            for (const HashNode &element : buckets[i]){     // 遍历桶(链表)里的每一个节点
                element.print();
            }
        }
        std::cout << std::endl;
    }

    void clear()
    {
        this->buckets.clear();          // 清空桶, 桶的类型是 std::vector<HashNode>
        this->numElements = 0;
        this->tableSize = 0;
    }
};



// 用来测试的主函数
int main() {
  // 创建一个哈希表实例
  HashTable<int, int> hashTable;

  int N;
  std::cin >> N;
  getchar();

  std::string line;
  for (int i = 0; i < N; i++) {
      std::getline(std::cin, line);
      std::istringstream iss(line);
      std::string command;
      iss >> command;
      
      int key;
      int value;

      if (command == "insert") {
          iss >> key >> value;
          hashTable.insert(key, value);
      }

      if (command == "erase") {
          if (hashTable.size() == 0) {
              continue;
          }
          iss >> key;
          hashTable.erase(key);
      }

      if (command == "find") {
          if (hashTable.size() == 0) {
              std::cout << "not exist" << std::endl;
              continue;
          }
          iss >> key;
          int* res = hashTable.find(key);
          if (res != nullptr) {
              std::cout << *res << std::endl;
          } else {
              std::cout << "not exist" << std::endl;
          }
      }

      if (command == "print") {
          if (hashTable.size() == 0) {
              std::cout << "empty" << std::endl;
          } else {
              hashTable.print();
          }
      }

      if (command == "size") {
          std::cout << hashTable.size() << std::endl;
      }

      if (command == "clear") {
          hashTable.clear();
      }
  }
  return 0;
}
```
- bucket: 桶, 在代码中是一个链表, 链表的节点为自定义的 `HashNode`.


在这个哈希表的实现中，冲突解决是通过`链地址法`（使用桶里的链表）来实现的。当一个键的哈希值映射到一个已经有其他键存在的桶时，这个键会被加入到桶对应的链表中。

需要注意的是，在 HashTable 的构造函数中，**buckets 数组被初始化为 size 长度两次，一次是在成员初始化列表中，另一次是在构造函数体中**。

## 值得注意的成员函数
rehash 方法:
```cpp
void rehash(size_t newSize) 
{
    std::vector<Bucket> newBuckets(newSize); // 创建新的桶数组

    for (Bucket &bucket : buckets) {      // 遍历旧桶
    for (HashNode &hashNode : bucket) {   // 遍历桶中的每个键
        size_t newIndex =
            hashFunction(hashNode.key) % newSize; // 为键计算新的索引
        newBuckets[newIndex].push_back(hashNode); // 将键添加到新桶中
    }
    }
    buckets = std::move(newBuckets); // 使用移动语义更新桶数组
    tableSize = newSize;             // 更新哈希表大小
}
```
> 当哈希表的负载因子超过预设的最大负载因子时，会调用这个私有成员函数来重新分配和初始化一个更大的桶数组，并重新将所有的键映射到新的桶中。这个过程称为重哈希（rehashing）。新桶的数量通常是当前桶数量的两倍。

insert 方法:
```cpp
void insert(const Key &key, const Value &value) 
{
    if ((numElements + 1) > maxLoadFactor * tableSize) { // 检查是否需要重哈希
    // 处理 clear 后再次插入元素时 tableSize = 0 的情况
    if (tableSize == 0) tableSize = 1;
    rehash(tableSize * 2); // 重哈希，桶数量翻倍
    }
    size_t index = hash(key);                     // 计算键的索引
    std::list<HashNode> &bucket = buckets[index]; // 获取对应的桶
    // 如果键不在桶中，则添加到桶中
    if (std::find(bucket.begin(), bucket.end(), key) == bucket.end()) {
    bucket.push_back(HashNode(key, value));
    ++numElements; // 增加元素数量
    }
}
```
> 首先检查是否需要重哈希，如果需要，则进行重哈希。
> 然后，它计算键的哈希值以找到对应的桶，如果键还不存在于桶中，则将其添加到桶的末尾，并增加元素计数。


erase 方法:
```cpp
void erase(const Key &key) 
{
    size_t index = hash(key);      // 计算键的索引
    auto &bucket = buckets[index]; // 获取对应的桶
    auto it = std::find(bucket.begin(), bucket.end(), key); // 查找键
    if (it != bucket.end()) {                               // 如果找到键
    bucket.erase(it);   // 从桶中移除键, 这里使用的是 vector 容器里的 erase()
      numElements--;    // 减少元素数量
    }
}
```
> 它计算键的哈希值，找到对应的桶，并在桶中遍历查找该键。找到后删除该键，并减少元素计数。


find 方法:
```cpp
Value *find(const Key &key) {
    size_t index = hash(key);      // 计算键的索引
    auto &bucket = buckets[index]; // 获取对应的桶 (引用类型)
    // 返回键是否在桶中
    auto ans = std::find(bucket.begin(), bucket.end(), key);
    if (ans != bucket.end()) {
    return &ans->value;             // 注意返回类型是 Value 指针
    };
    return nullptr;
}
```
> 这个方法用于查找一个键是否存在于哈希表中。
>> 它计算键的哈希值，找到对应的桶，然后在桶中使用 std::find() 查找该键。如果找到，则返回指针，否则返回空指针。

---


## 与STL标准库的区别
这里实现的只是一个简化的哈希表, 使用了`链表`来处理哈希冲突(这种方法也称为`分离链接法`). 以下是上面的实现与`std::unordered_set`或`std::unordered_map`的区别:
1. 模板参数:
    > 上面的实现只接受**键类型Key**和**哈希函数Hash**作为模板参数
    > STL的`std::unordered_set` 和 `std::unordered_map`有更多的模板参数, 比如键的类型, 值的类型, 哈希函数, 键的相等函数, 分配器等.

2. 负载因子和自动重哈希(rehash)
    > 上面的实现在负载因子超过 0.75 时将触发 rehash, 并且只能增加到当前大小的2倍.
    > STL的哈希表容器提供更多灵活性, 可以调整负载因子, 并且有更复杂的 rehash 策略.

3. 内存分配
    > 上面的稀疏是用 `std::list` 来管理冲突, 这**会导致多次单独的内存分配**.
    > STL通常使用更高效的内存分配策略, 比如预分配内存池来减少分配次数和提升性能.

4. 迭代器支持
    > 上述实现没有提供迭代器的支持, STL有完整的迭代器支持, 允许用户方便地遍历容器中的元素.

5. 功能丰富性:
    > 上述实现只有基础的插入, 删除和查找功能
    > STL的哈希表容器提供了丰富的接口, 如:
    >> emplace, count, bucket, bucket_count, bucket_size, load_factor, max_load_factor, rehash, reserve 等.

6. 异常安全性：
    > 上述实现没有显示地处理异常安全性问题。
    > STL的实现通常保证基本的异常安全性，并在某些操作中提供强异常安全性保证。

7. 优化：
    > 这里提供的是一个基本的哈希表，可能没有针对性能进行优化。  
    > STL的实现被高度优化以提供良好的性能，特别是在大数据量下。

8. 平台兼容性和移植性：
    > 上述实现理论上应该是跨平台的，但可能需要针对不同平台进行调整。
    > STL容器已经被广泛用于各种平台，并且被广大社区所测试，具有很高的兼容性和稳定性。

9. 接口一致性：
    > 上述实现可能没有遵守STL容器接口的标准命名和行为。
    > STL容器遵循一致的接口和命名规范，这有助于程序员理解和使用。

这些区别并不是说上述实现不好，而是说明了STL容器在设计时考虑到了许多复杂的场景和需求，使得它们在通用性、性能和易用性方面都非常强大。

用户定义的数据结构通常更适合解决特定问题或用例，而STL容器则为大多数常见情况提供了良好的默认选择。



---- 

# HashTable 常见的面试题

## 什么是哈希表？它是如何工作的？
> 哈希表是一种使用哈希函数组织数据，以便快速插入和搜索的数据结构。它通过将键映射到表中的位置来存储键值对。哈希函数将每个键转换为哈希表中的索引，该索引决定了键值对在表中的存储位置。如果两个键映射到同一个索引，就会产生冲突，这通常通过**链表**或**开放寻址法**来解决。
>> 开放地址法: 直接从冲突位置往后寻找下一个空闲的哈希表索引.


## 哈希冲突是什么？如何处理哈希冲突？

> 哈希冲突发生在不同的**键通过哈希函数映射到哈希表的同一位置**时。处理哈希冲突的方法有：
>> 链表法（分离链接法）：在每个哈希表索引上**维护一个链表**，所有映射到该索引的元素都会被存储在链表中。

>> 开放寻址法：如果发生冲突，就会**寻找下一个空闲的哈希表索引**。

>> 双重哈希：使用**一系列哈希函数**而不是单一哈希函数来确定元素的存储位置。

## 如何选择一个好的哈希函数？
> 一个好的哈希函数应该满足以下条件：
>> 快速计算。
>> 哈希值均匀分布，以减少冲突。
>> 一致性：相同的输入总是产生相同的输出。 不同的输入应尽可能映射到不同的输出。

## 什么是负载因子？对哈希表有什么影响？
> 负载因子是哈希表中**已存储元素数量与位置总数的比率**。它是衡量哈希表满程度的指标。
> 当负载因子过高时，冲突的可能性增加，这会降低哈希表的性能。因此，通常在负载因子达到一定阈值时，哈希表会进行扩容（即重哈希）来增加存储位置，从而减少冲突和维护操作的效率。

## 解释重哈希 (rehashing), 何时以及为什么需要重哈希？
> 重哈希是在哈希表的**负载因子超过预定的阈值**时，增加哈希表容量并重新分配现有元素的过程。
> 这个过程需要计算每个元素的新哈希值，并将它们移动到新表中的正确位置。
> rehash 可以帮助减少冲突和维持操作的快速性能。

## 在哈希表中插入、删除和搜索操作的复杂度是多少？
> 理想情况下，即没有发生冲突或冲突非常少时，插入、删除和搜索操作的时间复杂度为O(1)。
> 但是，在最坏的情况下，如果**所有的键都映射到同一索引**，则这些操作的时间复杂度会退化到O(n)，其中n是哈希表中元素的数量。
> 
> 使用良好的哈希函数和冲突解决策略可以帮助保持操作的**平均时间复杂度**为接近O(1)。

## 如何解决哈希表的扩容问题？
> 扩容通常发生在哈希表的负载因子超过预定阈值时。解决方案通常包括：

>> 创建一个更大的哈希表：创建一个容量更大的新哈希表。

>> 重新哈希所有元素：将所有现有的元素重新计算哈希值并插入到新的哈希表中。

>> 逐步迁移：在某些实现中，可以逐步迁移元素到新表，**分摊重哈希的成本到多次插入操作**中。

## 如何确保哈希表的线程安全
可以使用以下方式的一种来实现哈希表的线程安全:
- 互斥锁 (mutex)
    > 使用互斥锁来同步对哈希表的访问。**每次一个线程访问哈希表时，它都需要先获取锁**。

    - 代码实例 - `std::mutex`:
        ```cpp
        #include <iostream>
        #include <unordered_map>
        #include <mutex>

        template<typename K, typename V>
        class ThreadSafeHashTable {
        private:
            std::unordered_map<K, V> table;
            std::mutex mtx;  // 互斥锁是定义在整个哈希表里的, 因此整个哈希表使用单个锁来进行保护

        public:
            void insert(const K& key, const V& value) {
                std::lock_guard<std::mutex> lock(mtx);  // 先加锁, 再操作
                table[key] = value;
            }

            bool get(const K& key, V& value) {
                std::lock_guard<std::mutex> lock(mtx);  // 先加锁, 再操作
                auto it = table.find(key);
                if (it != table.end()) {
                    value = it->second;
                    return true;
                }
                return false;
            }
        };
        ```


- 读写锁 (read-write lock)
    > 如果 `读操作` 远多于 `写操作`, 使用**读写锁可以提高性能**，因为它允许多个线程同时读取，但`写入时需要排他访问`。
    - 代码实例 - `std::shared_mutex`
        ```cpp
        #include <iostream>
        #include <unordered_map>
        #include <shared_mutex>

        template<typename K, typename V>
        class ThreadSafeHashTable {
        private:
            std::unordered_map<K, V> table;
            std::shared_mutex rw_mtx;

        public:
            void insert(const K& key, const V& value) {
                std::unique_lock<std::shared_mutex> lock(rw_mtx);  // 加锁, 由于是写操作, 这里用的是 unique_lock
                table[key] = value;
            }

            bool get(const K& key, V& value) {
                std::shared_lock<std::shared_mutex> lock(rw_mtx);  // 加锁, 由于是可共享的读操作, 用的是 shared_lock
                auto it = table.find(key);
                if (it != table.end()) {
                    value = it->second;
                    return true;
                }
                return false;
            }
        };
        ```


- 原子操作 (atomic operation)
    > 原子操作是**不可中断的操作**，通常用于实现**无锁编程**. 
    > 对于简单的操作，可以使用原子操作来避免使用锁。

    - 代码实例 - `std::atomic<数据类型>`为每一个 bucket 增加上锁的功能
        ```cpp
        #include <iostream>
        #include <unordered_map>
        #include <atomic>
        #include <vector>
        #include <thread>

        template<typename K, typename V>
        class ThreadSafeHashTable {
        private:
            std::unordered_map<K, V> table;
            std::vector<std::atomic<int>> locks;   // 用于实现自旋锁的原子变量数组，每个桶（bucket）对应一个原子锁

        public:
            ThreadSafeHashTable(size_t size) : locks(size) {
                for (auto& lock : locks) {  
                    lock.store(0); // 为每个原子锁设置初始值0（表示未上锁）
                }
            }

            void insert(const K& key, const V& value) {

                int hash = std::hash<K>{}(key) % locks.size(); 
                /*
                    std::hash<K>{} 创建了一个临时对象, 目的只是为了使用它的哈希方法; 
                    std::hash<K>{}(key) 里的(key) 相当于调用了成员函数里的"()"操作符方法, 也就是用了仿函数的调用方式.
                */
                
                while (locks[hash].exchange(1) == 1); // Spinlock
                /*
                exchange(1) 尝试将锁的值设置为1，并返回旧值。
                如果旧值也是1，则表示锁已被其他线程持有，需要继续自旋等待, 代码的执行流程继续卡在while这一行里.
                */

                table[key] = value;
                locks[hash].store(0);  // 释放锁 
            }

            bool get(const K& key, V& value) {
                int hash = std::hash<K>{}(key) % locks.size();
                while (locks[hash].exchange(1) == 1); // Spinlock  (解释同上)
                auto it = table.find(key);
                if (it != table.end()) {
                    value = it->second;
                    locks[hash].store(0); // 释放这个桶的锁
                    return true;
                }
                locks[hash].store(0);     // 走到这里说明没找到那个key, 也要释放锁
                return false;
            }
        };
        ```

- 细粒度锁 (fien-grained lock)
    > 细粒度锁是一种锁的策略，旨在将锁的范围细化到更小的粒度，以提高并发度。
    > 相对于粗粒度锁 (保护**大块代码**或**多个资源**的锁), **细粒度锁只保护小块代码或单个资源**。例如，如果有多个资源，可以为每个资源分别使用不同的锁，而不是使用一个大锁保护所有资源。 
    >> 不是对整个哈希表加锁，而是对哈希表的一部分（例如单个桶或链表）加锁，以减少锁的粒度。

    - 代码实例 - `std::mutex` 为每个桶(或一部分)使用单独的锁.
        ```cpp

        #include <iostream>
        #include <unordered_map>
        #include <vector>
        #include <mutex>

        template<typename K, typename V>
        class ThreadSafeHashTable {
        private:
            struct Bucket {    // 每个桶有一个std::mutex，用于同步对桶中数据的访问
                std::unordered_map<K, V> bucket;
                std::mutex mtx; // 注意这里的 mutex 是定义在桶类型中的, 意味着每个桶有一个互斥锁
            };

            std::vector<Bucket> buckets;
            size_t bucket_count;

        public:
            ThreadSafeHashTable(size_t bucket_count) : bucket_count(bucket_count), buckets(bucket_count) {}

            void insert(const K& key, const V& value) {
                size_t bucket_index = std::hash<K>{}(key) % bucket_count;
                std::lock_guard<std::mutex> lock(buckets[bucket_index].mtx);  // 锁定待操作的桶;  使用std::lock_guard锁定对应的桶，以确保插入操作的线程安全。
                buckets[bucket_index].bucket[key] = value;  // 上锁成功后执行对应操作
            }

            bool get(const K& key, V& value) {
                size_t bucket_index = std::hash<K>{}(key) % bucket_count;
                std::lock_guard<std::mutex> lock(buckets[bucket_index].mtx);   // 锁定待操作的桶,  使用std::lock_guard锁定对应的桶，以确保读取操作的线程安全。
                auto it = buckets[bucket_index].bucket.find(key);
                if (it != buckets[bucket_index].bucket.end()) {
                    value = it->second;
                    return true;
                }
                return false;
            }
        };
        ```
        - 关键点:
          - 细粒度锁：通过为`每个桶分配一个独立的锁`，**允许多个线程同时操作不同的桶**，从而**减少锁的争用**，提高并发性能。
          - 互斥锁：使用std::mutex 和 std::lock_guard 确保对每个桶的操作是线程安全的。
            - `std::lock_guard`在**构造时自动上锁，在析构时自动解锁**，避免了手动管理锁的开销和潜在的错误。


## 有哪些常见的哈希表实现问题？
常见的哈希表实现问题包括：
> 内存使用不当：如果**哈希表过大**或者存在**许多空桶**，可能会导致内存浪费。

> 冲突处理不佳：如果**冲突没有得到有效处理**，会严重影响哈希表的性能。

> 哈希函数选择不当：一个**不好的哈希函数**可能会导致频繁的冲突。

> 扩容代价高：rehash 是一个代价很高的操作，如果**发生得太频繁，可能会严重影响性能**。
