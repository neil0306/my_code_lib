# List

`std::vector` 和 `std::list` 是两种不同的数据结构, vector是基于数组的动态数组, 而 std::list 是**基于双向链表的数据结构**.

list 适用于需要在序列中执行频繁插入、删除操作的场景.

## list 的特性
- 双向链表
  - list 是一个双向链表, 允许在序列的`两端和中间执行高效的插入和删除操作`.

- 不支持随机访问
  - list 与 vector、deque 不同, list不支持通过索引进行`常量时间内的随机访问`, 要访问list中的元素, **必须通过迭代器进行**.

- 动态内存管理
  - list 的内部实现使用节点, 每一个节点都包含一个元素和指向前后节点的指针 (双向链表节点的特征). 这种结构使得 list 在执行插入和删除操作时能更好地管理内存.

- 保持迭代器的有效性
  - list 在进行插入和删除操作时, 能够更好地保持迭代器的有效性. 这意味着在进行这些操作后, 不会导致所有迭代失效.
    - vector的迭代器容易在执行 reserve 操作后失效 (插入元素的时候如果触发扩容, 则会调用 reserve, 导致迭代器失效)

- 高效的插入和删除操作
  - 由于list是双向链表, 插入和删除操作在两端、中间都是**常量时间**的, 这使得它成为增删频繁场景下的理想容器.

## list的性能考虑
- 插入和删除
  - 如果主要进行频繁的删除和操作, 并不需要进行随机访问, 那么使用 list 要比 vector 更好
- 随机访问:
  - 如果需要通过索引进行随机访问元素, 使用vector会更合适, 因为它能以 $O(1)$ 的时间复杂度进行访问
- 内存使用: 
  - 由于 list 使用了链表结构, 可能引入一些额外的内存开销, 在内存使用方面, vector 和 deque 可能更为紧凑.


## C++ 标准库中list的基本用法
```cpp
#include <list>  // 引入头文件

int main(void)
{
    std::list<int> myList;  // 声明一个list对象

    myList.push_back(1);     // 后端插入一个元素
    myList.push_front(2);    // 前端插入一个元素

    myList.pop_back();       // 删除后端的一个元素
    myList.pop_front();      // 删除前端的一个元素
    return 0;
}
```


## list 的工作原理
![](list_images/list在内存中的处理.png)

图例说明
- 蓝色矩形框：堆内存
- 红色矩形块：栈内存
- 红色箭头：next指针（参见实现）
- 蓝色箭头：prev指针（参见实现）

图中展示了一个list从初始化到完成多次插入的流程。List类也有一个控制结构, 包含`head`, `tail`, `size`三个成员, 分别控制链表的头尾指针和大小, 让我们梳理其生命流程:
1. 初始时刻, 其链表为空, size=0, 均为空指针, 只有这些**控制结构**存储在`栈上`

2. 执行`push_back(1)`, 在堆内存中分配了一个Node(参考后文实现, 其实就是个双向链表节点), 将head, tail指向这个节点, 更新size=1

3. 执行`push_back(2)`, 在堆内存中分配了一个Node, 需要注意的是, 这个新的节点不需要和旧的节点内存连续, 更新原tail指向节点的next, 将tail指向这个节点, 并且将新的节点的prev指向原来的tail, 更新size=2

4. 执行`push_back(3)`, 操作与3相同, 此过程完成后, size=3

5. 执行`push_front(3)`, 在堆内存中分配了一个Node, 需要注意的时, 这个新的节点不需要和旧的节点内存连续, 此时更新原head指向节点的prev, 将head指向这个节点, 并且将新的节点的next指向原来的head, 更新size=4

# 实现一个简单的list
```cpp
// #pragma once 
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <string>

template <typename T>
class List
{
public:
    template <typename L>
    friend std::ostream &operator<<(std::ostream &os, const List<L> &pt); // 友元函数

private:  // private区域定义受保护的数据成员
    struct Node
    {
        T data;         // 数据指针
        Node *next;     // 指向下一个节点
        Node *pre;      // 指向前一个节点

        // 构造函数
        Node (const T &value, Node *nextNode = nullptr, Node *preNode = nullptr) : data(value), next(nextNode), pre(preNode){} 
    };

    Node *head;         // List 类的头节点指针
    Node *tail;         // List 类的尾节点指针
    size_t size;        // List 中节点的数量

public:   // public区域定义各类功能函数
    // 构造函数
    List() : head(nullptr), tail(nullptr), size(0) {}

    // 析构函数
    ~List() { clear(); }  // 在下面定义了一个 clear 函数来清理节点

    void push_back(const T &value)
    {
        // 1. 创建新节点
        Node *newNode = new Node(value, nullptr, tail); // pre指向未插入元素前的tail节点

        if (tail){  // List 非空, 只需要更新尾指针
            tail->next = newNode;  
        }
        else{       // List 为空, 只需要更新头指针
            head = newNode;  
        }

        // 更新尾指针和链表大小
        tail = newNode;
        ++size;
    }

    void push_front(const T &value)
    {
        Node *newNode = new Node(value, head, nullptr); // next 指向原有的head

        if (head){ // List 不为空
            head->pre = newNode;
        }
        else{      // List 为空
            tail = newNode;
        }

        head = newNode;
        ++size;
    }

    size_t getSize() const
    {
        return size;
    }

    // 注意: std::list 没有提供[]运算符重载! 因为它不能保证O(1)的时间复杂度!
    // 访问List中的元素 (并非随机访问, 需要寻址)  
    T &operator[](size_t index)
    {
        Node *cur = head;   // 用于寻址
        for(size_t i = 0; i < index; ++i){
            if (!cur){      // 用户指定的index超出链表长度
                throw std::out_of_range("index out of range!");
            }
            cur = cur->next;
        }
        return cur->data;
    }

    const T &operator[](size_t index) const
    {
        Node *cur = head;
        for(size_t i = 0; i < index; ++i){
            if(!cur){
                throw std::out_of_range();
            }
            cur = cur->next;
        }
        return cur->data;
    }

    void pop_back()
    {
        if(size > 0){
            // 获取尾节点的前一个节点, 防止指针丢失
            Node * newTail = tail->pre;

            // 删除尾节点
            delete tail;

            tail = newTail;   // 更新尾节点指针
            if(tail){  // 非空指针
                tail->next = nullptr;
            }
            else {      // 空指针 (如果原来的List只有一个节点, tail就会是空的)
                head = nullptr;
            }

            -- size;
        }
    }

    void pop_front()
    {
        if(size > 0){
            Node *newHead = head->next;

            delete head;

            head = newHead;
            if(head){
                head->pre = nullptr;
            }
            else{
                tail = nullptr;
            }

            --size;
        }
    }

    // 获取指定值的节点
    void *getNode(const T &val){
        Node *node = head;
        while(node != nullptr && node->data != val){
            node = node->next;
        }
        return node;
    }

    // 或者指定值的节点的 数据指针
    T *find(const T &val){
        Node *node = getNode(val);
        if(node == nullptr){
            return nullptr; 
        }
        return &node->data;
    }

    // 删除指定节点
    void remove(const T &val)
    {
        Node *node = head;

        // 找待删除节点
        while(node != nullptr && node->data != val){
            node = node->next;
        }

        if(node == nullptr){  // 没找到目标
            return;
        }

        if(node != head && node != tail){       // 非头尾指针
            // 直接绕开被删节点
            node->pre->next = node->next;   
            node->next->pre = node->pre;
        }
        else if(node == head && node == tail){  // 是头节点也是尾节点
            head = nullptr;
            tail = nullptr;
        }
        else if(node == head){                  // 是头节点
            head = node->next;
            head->pre = nullptr;
        }
        else{                                   // 是尾节点
            tail = node->pre;
            tail->next = nullptr;
        }

        --size;
        delete node;        // 别忘了释放内存
        node = nullptr;     // 防止出现垂悬指针
    }

    bool empty() { return size == 0; }

    // 清空链表
    void clear()
    {
        while(head)
        {
            Node *temp = head;
            head = head->next;      // 先更新头节点, 防止地址丢失
            delete temp;
        }

        // 走到这里就删完了, 更新其他控制成员
        tail = nullptr;
        size = 0;
    }

    // 迭代器
    Node *begin() { return head; }
    
    // 需要特别注意:  end() 返回一个指向链表末尾之后位置的迭代器，这个位置被称为“past-the-end”元素。
    // 这个迭代器不指向任何实际的元素，因此不能被解引用
    Node * end() { return nullptr; }       
    
    // const 版本 (保证只读模式)
    const Node *begin() const { return head; }
    const Node *end() const { return nullptr; }   // 同上, 都是past-the-end元素, 不能被解引用

    // 打印链表中的元素
    void printElements() const
    {
        for (Node *cur = head; cur != nullptr; cur = cur->next){
            std::cout << cur->data << " ";
        }
        std::cout << std::endl;
    }
};

// 运算符重载 (前面已经声明为友元函数)
template<typename T>
std::ostream &operator<<(std::ostream os, const List<T> &pt)
{
    
    // for(typename List<T>::Node *cur = pt.head; cur != nullptr; cur = cur->next){  // 访问List类中的结构体, 需要用 typename List<T>::Node
    //     os << " " << cur->data;
    // }
    // os << std::endl;
    // return os;

    typedef typename List<T>::Node myNode;
    for(myNode *cur = pt.head; cur != nullptr; cur = cur->next){  // 访问List类中的结构体, 需要用 typename List<T>::Node
        os << " " << cur->data;
    }
    os << std::endl;
    return os;
}

```

测试函数:
```cpp
int main() {
    // 创建一个 List 对象
    List<int> myList;
    
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
            myList.push_back(value);
        }

        if (command == "push_front") {
            iss >> value;
            myList.push_front(value);
        }

        if (command == "pop_back") {
            myList.pop_back();
        }

        if (command == "pop_front") {
            myList.pop_front();
        }

        if (command == "remove") {
            iss >> value;
            myList.remove(value);
        }

        if (command == "clear") {
            myList.clear();
        }

        if (command == "size") {
            std::cout << myList.getSize() << std::endl;
        }

        if (command == "get") {
            iss >> value;
            std::cout << myList[value] << std::endl;
        }

        if (command == "print") {
            if (myList.getSize() == 0) {
                std::cout << "empty" << std::endl;
            } else {
                myList.printElements();
            }
        }
    }
    return 0;
}
```

测试用例:
```txt
17
push_back 10
push_back 20
push_front 30
push_front 40
size
print
get 1
pop_back
print
pop_front
print
remove 10
print
size
clear
print
size
```

## 分析
### 析构函数
```cpp
~List() {
    clear();
}
void clear() {
    while (head) {
        // 从头节点开始，依次删除节点
        Node* temp = head;
        head = head->next;
        delete temp;
    }

    // 更新尾节点指针和链表大小
    tail = nullptr;
    size = 0;
}
```
析构函数在销毁链表对象时调用 clear() 方法，删除所有节点，释放内存。clear() 方法简单地从头结点开始清空内存, `清空前需要记录next指针`.
> 可以用智能指针 `shared_ptr` 来避免显式的空间回收.

### push_back
```cpp
void push_back(const T& value) {
    // 创建新的节点
    Node* newNode = new Node(value, nullptr, tail);

    if (tail) {
        // 如果链表非空，将尾节点的 next 指针指向新节点
        tail->next = newNode;
    } else {
        // 如果链表为空，新节点同时也是头节点
        head = newNode;
    }

    // 更新尾节点指针和链表大小
    tail = newNode;
    ++size;
}
```
在链表的尾部添加一个新的元素, new节点时就设置prev指针为原来的tail, 需要通过tail判断链表是否为空, 如果为空还需要设置head指向新节点, 因为此时第一个链表节点也是最后一个节点, 最后将tail指向新节点.
> 易错点: 容易忘记判断链表是否为空.

push_front 方法只需要将 tail 和 head 换个位置.

### operator[ ]
```cpp
T& operator[](size_t index) {
    // 从头节点开始遍历链表，找到第 index 个节点
    Node* current = head;
    for (size_t i = 0; i < index; ++i) {
        if (!current) {
            // 如果 index 超出链表长度，则抛出异常
            throw std::out_of_range("Index out of range");
        }
        current = current->next;
    }

    // 返回节点中的数据
    return current->data;
}
```
这个方法就是`[]`的重载, 通过索引访问链表中的元素，返回对应节点的数据引用。逻辑就是从头开始遍历链表, 也很简单, 但需要处理越界的情况.

> 易错点：
处理越界的情况, 此处的处理是抛出异常。
需要注意返回的是引用, 否则就不能通过`[]`更新元素.


### pop_back
```cpp
void pop_back() {
    if (size > 0) {
        // 获取尾节点的前一个节点
        Node* newTail = tail->prev;

        // 删除尾节点
        delete tail;

        // 更新尾节点指针和链表大小
        tail = newTail;
        if (tail) {
            tail->next = nullptr;
        } else {
            head = nullptr;  // 如果链表为空，头节点也置为空
        }

        --size;
    }
}
```
删除链表尾部的元素, 删除前需要获取原来尾结点的前一个节点, 将其置为删除后的新tail, 但还需要判断删除后链表是否为空, **为空的话需要将头结点也置为空, 否则head是一个悬垂引用**.
> 补充知识: 悬垂引用 悬垂引用（Dangling Reference）在C++中指的是一种**指向已经释放或者不再有效内存的引用**。悬垂引用的存在**可能导致未定义行为**，比如程序崩溃、数据损坏或者难以追踪的bug。

> 改进点： 可以考虑使用智能指针 shared_ptr 以避免显式的空间回收.


### pop_front
```cpp
void pop_front() {
    if (size > 0) {
        // 获取头节点的下一个节点
        Node* newHead = head->next;

        // 删除头节点
        delete head;

        // 更新头节点指针和链表大小
        head = newHead;
        if (head) {
            head->prev = nullptr;
        } else {
            tail = nullptr;  // 如果链表为空，尾节点也置为空
        }

        --size;
    }
}
```
删除链表头部的元素。其实就是pop_back的反向逻辑, 只需要将push_back中的tail和head变换位置即可

>易错点：
>忘记在每次删除节点后更新 head 指针。
>忘记将 size 重置为0。


### 迭代器: begin 和 end
```cpp
Node* begin() {
    return head;
}

// 使用迭代器遍历链表的结束位置
Node* end() {
    return nullptr;
}

// 使用迭代器遍历链表的开始位置（const版本）
const Node* begin() const {
    return head;
}

// 使用迭代器遍历链表的结束位置（const版本）
const Node* end() const {
    return nullptr;
}
```
目前的实现中, 迭代器就是指针, **有const和non const的版本**。`begin()` 返回指向链表第一个元素的指针。`end()` 返回一个**尾后迭代器**，通常是 `nullptr`，表示链表的末尾。


# 高频面试题

## 移除所有指定元素

题目：
```txt
给定一个 std::list<int>，编写一个函数 removeAllInstances，该函数移除列表中所有值等于特定元素的节点。
```

参考答案：

在C++中，std::list `提供了 remove()` 成员函数来移除所有等于特定值的元素。如果需要手动实现这一功能，可以**使用迭代器遍历列表**，然后使用 `erase()` 方法删除当前迭代器指向的元素。下面提供了一个参考实现：
```cpp
#include <list>
#include <iostream>

template <typename T>
void removeAllInstance(std::list<T>& lst, const T& value)
{
    for (auto it = lst.begin(); it != lst.end();){      // 注意这里没有自增
        if( *it == value){          // 这里比较的是传入的 T 类型的对象
            it = lst.erase(it);     // erase 会返回下一个元素的迭代器
        }
        else{
            ++it;   // 指针自增, 相当于移动到下一个元素位置
        }
    }
}

int main() {
    std::list<int> mylist = {1, 2, 3, 4, 3, 2, 1};

    removeAllInstances(mylist, 2);  // Remove all instances of '2'

    for (int n : mylist) {
        std::cout << n << ' ';  // Should print: 1 3 4 3 1
    }

    return 0;
}
```

这段代码定义了一个**泛型函数(模板函数)** removeAllInstances，它接受一个 list 和一个要移除的值 value。通过迭代遍历 list，当找到与 value 相等的元素时，使用 erase 移除它，并确保迭代器指向下一个元素。需要注意的是，`erase()` 方法**会使传入的迭代器失效**，但**它会返回指向被删除元素之后元素的有效迭代器**。

> 面试中，除了提供上述代码实现外，还可以讨论 std::list 的特性，例如它的**元素不是连续存储**的，因此它的**随机访问效率较低**，但是**插入和删除操作效率较高**，**不会导致迭代器失效（除了指向被删除元素的迭代器）**。这些特性使得 std::list 在需要频繁插入和删除但不需要随机访问的场景中非常有用。


## STL list 的特性

题目：
```txt
描述 C++ STL list 的特性，包括其内部工作机制和用途。
```

参考答案：

C++ STL list 是一个**序列容器**，它允许**非连续内存分配**。以下是 list 的一些主要特性：

- 双向链表：list 是一个双向链表，允许从两个方向遍历。

- 非连续内存：由于它是链表，元素不存储在连续的内存位置。这意味着**除了通过迭代器外，无法通过常规索引来访问元素**。

- 动态大小：与数组不同，list **可以根据需要动态增长或缩减**。

- 插入和删除性能：在任意位置插入和删除元素都非常快，`时间复杂度为 O(1)`。

- 不支持随机访问：由于其内部实现，list 不支持快速随机访问，所以`访问元素的时间复杂度为 O(n)`。

- 额外内存开销：**每个元素都需要额外的内存来存储前后元素的指针**。

list 通常用于以下用途：
- 当需要频繁在列表中间插入或删除元素时。
- 当不需要快速随机访问元素时。
- 当需要**保证迭代器在插入和删除操作后不失效时**。


## list 的迭代器失效的情况

题目：
```txt
在对 STL list 进行插入和删除操作时，哪些情况下迭代器会失效？
```

参考答案：

对于 STL list 来说，迭代器失效的情况相对较少。由于 list 是一个双向链表，迭代器在插入和删除操作之后通常仍然有效。具体来说：

- 插入操作：在 list 中插入操作不会导致任何现有迭代器失效，包括指向插入位置的迭代器。插入操作后，原来的迭代器仍然指向它们原来指向的元素。
- 删除操作：**删除操作会导致指向被删除元素的迭代器失效**。然而，其他迭代器，包括指向前一个和后一个元素的迭代器，仍然有效。


## list 与 vector 的比较
题目：
```txt
比较 C++ STL list 和 vector，它们的优势、劣势和适用场景是什么？
```

参考答案：

list 和 vector 是 C++ STL 中的两种常用序列容器，它们各有优缺点。

- 内部实现：
  - list 是一个双向链表，不支持随机访问。
  - vector 是一个动态数组，支持快速随机访问。

- 性能特点：
  - vector：
    > 支持随机访问，可以通过索引以 O(1) 时间复杂度访问任意元素。
    > 尾部插入和删除操作快（通常是 O(1)），但在中间或开头插入或删除元素需要移动后续元素，可能导致 O(n) 时间复杂度。
    > 当超出当前容量时，需要重新分配内存并复制所有元素到新空间，这是一个相对昂贵的操作。
  - list：
    > 不支持随机访问，访问特定元素需要 O(n) 时间复杂度。
    > 在任意位置插入和删除操作都很快（O(1)），因为只需要改变指针。
    > 不需要重新分配整个容器的内存空间，因为它不是连续存储的。

- 遍历- 内部实现：
  - vector 是基于连续内存空间的**动态数组**实现，这意味着它的元素存储在一个**连续的内存块**中。
  - list 是基于**双向链表**实现的，它的每个元素都是单独的内存块，通过指针连接。

- 内存使用：
  - vector 通常有较小的内存开销，因为**它不需要为每个元素存储额外的指针**。
  - list 对于每个元素都需要存储两个额外的指针（前驱和后继），这意味着更高的内存开销。

- 迭代器失效：
  - vector 的迭代器在**重新分配内存后**可能会失效，或者在除了尾部之外的任何位置进行插入或删除操作时会失效。
  - list 的迭代器在插入和删除操作后依然有效，**除了被删除元素的迭代器外**。

- 适用场景：
  - vector 适合用于元素数量固定或仅在尾部进行添加和删除操作的场景，以及需要频繁随机访问元素的场景。
  - list 适合用于元素数量经常变动，特别是需要在列表中间频繁进行插入和删除操作的场景。

