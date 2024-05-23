# deque
deque 是STL里的一种`双端队列结构`, 它支持在两端添加和删除元素.

## deque 的特性
- 双端操作:
  - deque 支持在前端和后端执行快速插入和删除操作, 适用于经常需要**在前后两端增删元素的场景**.

- 随机访问:
  - deque 支持常量时间内的随机访问, 即`支持使用元素下标直接访问元素, 并且时间复杂度为 O(1)`.
    - deque 能支持随机访问是因为内部使用了`分段数组`, 每个分段都是一个固定大小的数组, 因此**可以直接计算索引的位置, 而不需要遍历整个容器**. 
      - 虽然 deque 允许常量时间内进行随机访问, 但与`std::vector`相比, `std::deque`在访问元素时可能涉及更多的间接层次, 因此, **在某些情况下 vector 的性能可能会更优秀一些**.
    - list 的内部实现是链表, 因此使用元素下标访问时需要使用迭代器遍历, 时间复杂是 `O(n)`

    ```cpp
    #include <iostream>
    #include <deque>
    using namespace std;

    int main(void)
    {
        std::deque<int> myDeque = {1, 2, 3, 4, 5};

        // 随机访问的时间复杂为 O(1)
        cout << "Element at index 2: " << myDeque[2] << endl;
        cout << "Element at index 4: " << myDeque[4] << endl;

        return 0;
    }
    ```


- 动态扩展:
  - deque 的大小可以动态调整, 无需事先分配固定大小的内存.
  - deque 适用于需要动态增长和缩小的情况.

- 保持迭代器有效性
  - deque 在进行插入和删除操作时, 能更好地**保持迭代器的有效性**. 进行增删元素之后, 不会导致所有迭代器失效.

- 内存局部性:
  - deque 内部结构利用了**多个缓冲区**, 有助于提高内存局部性, 从而在某些情况下提供更好的性能.

## STL 中 deque 的使用方法

```cpp
// 包含头文件
#include <deque>   

// 声明一个 deque 对象
std::deque<int> myDeque;

// 使用 push_front 和 push_back 在前、后端插入元素
myDeque.push_front(1);
myDeque.push_back(2);

// 使用 pop_front 和 pop_back 从 deque 的前、后端删除元素
myDeque.pop_front();
myDeque.pop_back();
```

## deque 的工作原理
STL 中的 deque (双端队列) 规定了接口和时间复杂度, 但**没有规定特定的实现细节**. C++ 标准描述了每个容器的行为和性能要求, 具体实现留给了实现者. 

因此, 这里介绍的只是一种实现方法:
- 定义一个模板类, 内部使用`循环数组`来模拟双端队列.

### 总体思路
数据结构：
- elements: 动态数组存储队列元素
- capacity: 数组容量
- size: 数组长度(已使用的容量)
- frontIndex 和 backIndex : 分别指向队列的前端和后端元素。
  > 注意: 
    >> 此处的 frontIndex 和 backIndex **类似标准STL中的 begin 和 end** 
    >> frontIndex 位置指向的数据是已存在的 (除了deque为空的情况), 
    >> backIndex 指向的位置是**当前末尾元素的下一个位置**, 也就是**还没有有效的数据** (**如果容量满了, 其会指向frontIndex的位置**)
- 循环数组：通过模运算来实现数组的循环效果，使得队列可以在数组的任意一端进行插入和删除操作, 因为**索引越界后直接取模从另一端开始就可以了**。
- 动态调整大小：当数组达到容量限制时，**会自动扩展容量（加倍扩展）**，并**将所有现有元素复制到新的更大的数组**中。
- 索引计算：利用**模运算**来正确计算新的 frontIndex 和 backIndex，无论是添加或删除操作。

图示- 前后位置插入元素:
![](deque_images/deque在两端插入的逻辑.png)
- 无论是 `push_front` 还是 `push_back`, 都需要判断当前数组是否已满, 已满的话就申请一块新的内存, 大小为原来的2倍, 然后将原来的数组元素拷贝到新的数组中.
- push_front 需要将 frontIndex 自减之后, 在 frontIndex 位置插入;
  - **frontIndex 自减步骤中, 自减后需要 加上 capacity**, 然后对 capacity 取模, 这可以保证当 frontIndex 自减为负时, 计算后将指向数组末尾.
- push_back 直接在 index_back 位置插入, 然后将 backIndex 自增, 然后对 capacity 取模
  -  取模这一步确保了如果自增后的 backIndex 越界的话, 就会指向数组开始的位置.


--- 
# 实现 deque

```cpp
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <string>

template <typename T>
class Deque
{
private:  // 定义内部用于控制状态的成员
    T* elements;      // 动态数组指针
    size_t capacity;  // 动态数组总容量
    size_t size;      // 动态数组中元素的个数
    size_t frontIndex; // 前端 index
    size_t backIndex;  // 末尾 index

public:
    // 构造函数
    Deque() : elements(nullptr), capacity(0), size(0), frontIndex(0), backIndex(0) {}

    // 析构函数
    ~Deque()
    {
        clear();             // 逐一清空元素, 注意这里不负责释放数组元素指针
        delete[] elements;   // 释放动态数组指针
    }

    // 清空 deque 元素, 注意动态数组指针还在
    void clear()
    {
        while(size > 0){
            pop_front();
        }
    }

    // deque 前端插入元素
    void push_front(const T& value)
    {
        // 判断数组是否已满
        if(size == capacity){
            resize();           // 扩容 + 搬数据 + 更新所有deque的状态成员
        }

        // 计算前端 index
        frontIndex = (frontIndex - 1 + capacity) % capacity; // 加 capacity 后取模, 防止出现负数

        // 放入新元素
        elements[frontIndex] = value;

        ++size;
    }

    // deque 末尾插入元素
    void push_back(T& value)
    {
        // 检查容量
        if(size == capacity){
            resize();
        }

        // 先插入新元素 (因为 backIndex 本身就指向能直接插入元素的未使用位置)
        elements[backIndex] = value;

        // 计算末尾 index
        backIndex = (backIndex + 1) % capacity;  // 取模, 防止越界

        ++size;
    }

    // 从 deque 的前端移除元素
    void pop_front()
    {
        // 判断是否为空
        if(size == 0){
            throw std::out_of_range("Deque is empty");
        }

        // 删除元素并不需要释放空间, 只需要修改 index 即可, 后面进来的新元素会覆盖旧的
        frontIndex = (frontIndex + 1) % capacity;  // 取模, 防止越界

        --size;
    }

    // 从 deque 末尾移除元素
    void pop_back()
    {
        // 判断是否为空
        if(size == 0){
            throw std::out_of_range("Deque is empty");
        }

        // 删除元素并不需要释放空间, 只需要修改 index 即可, 后面进来的新元素会覆盖旧的
        backIndex = (backIndex - 1 + capacity) % capacity; // 防止出现负数

        --size;
    }

    // 随机访问元素
    T& operator[](int index)
    {
        if(index < 0 || index >= size){
            throw std::out_of_range("Index out of range");
        }
        return elements[(frontIndex + index) % capacity];  // 注意取元素得从 frontIndex 开始计算, 取模防止越界
    }

    // 获取元素数量
    size_t getSize() const
    {
        return size;
    }

    void printElements() const
    {
        size_t index =frontIndex;
        for(size_t i = 0; i < size; ++i){
            std::cout << elements[index] << " ";
        }
        std::cout << std::endl;
    }

private:
    // 调整容量
    void resize()
    {
        // 计算新容量大小
        size_t newCapacity = (capacity == 0) ? 1 : 2 * capacity;

        // 创建新数组
        T* newElements = new T[newCapacity];

        // 复制旧元素
        size_t index = frontIndex;         // index 是旧数组下标
        for(size_t i = 0; i < size; ++i){  // i 是新数组下标
            newElements[i] = elements[index];
            index = (index + 1) % capacity; // 更新 & 防止越界
        }

        // 释放旧数组
        delete[] elements;

        elements = newElements; // 更新数组指针
        capacity = newCapacity; // 更新容量
        frontIndex = 0;         // 对于新数组而言, 0 就是开头
        backIndex = size;       // size 刚好指向数组末端有效元素的下一个位置
    }
};

```

测试函数:
```cpp
int main() {
        // 创建一个 Deque 对象
    Deque<int> myDeque;

    int N;
    std::cin >> N;
    // 读走回车
    getchar();
    std::string line;
    // 接收命令
    for (int i = 0; i < N; i++) {
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        int value;

        if (command == "push_back") {
            iss >> value;
            myDeque.push_back(value);
        }

        if (command == "push_front") {
            iss >> value;
            myDeque.push_front(value);
        }

        if (command == "pop_back") {
            if (myDeque.getSize() == 0) {
                continue;
            }
            myDeque.pop_back();
        }

        if (command == "pop_front") {
            if (myDeque.getSize() == 0) {
                continue;
            }
            myDeque.pop_front();
        }

        if (command == "clear") {
            myDeque.clear();
        }

        if (command == "size") {
            std::cout << myDeque.getSize() << std::endl;
        }

        if (command == "get") {
            iss >> value;
            std::cout << myDeque[value] << std::endl;
        }

        if (command == "print") {
            if (myDeque.getSize() == 0) {
                std::cout << "empty" << std::endl;
            } else {
                myDeque.printElements();
            }
        }
    }
    return 0;
}
```

测试用例:
```txt
输入:

15
push_back 20
push_back 10
push_front 30
push_front 40
size
print
pop_back
print
pop_front
print
size
get 1
clear
print
size


--- 
输出:

4
40 40 40 40 
40 40 40 
30 30 
2
20
empty
0
```







