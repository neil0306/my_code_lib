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












