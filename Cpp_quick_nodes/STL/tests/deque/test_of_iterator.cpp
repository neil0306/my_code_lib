#include <iostream>
#include <deque>

using namespace std;

void printDeque(const deque<int>& myDeque)
{
    for (auto i : myDeque){
        cout << i << " ";
    }
    cout << endl;
    cout << "-----------------------" << endl;
}

int main(void)
{
    // basic
    deque<int> myDeque = {1, 2, 3, 4, 5};
    cout << "Original deque: ";
    printDeque(myDeque);
    
    deque<int>::iterator it_first = myDeque.begin();
    deque<int>::iterator it_last = myDeque.end() -1;  // 注意要减1

    // insert a new element to the front
    cout << "Before insert, the first element is: " << *it_first << endl;
    cout << "Before insert, the last element is: " << *it_last << endl;
    myDeque.push_front(10000);              // 插入新元素
    cout << "After insert, myDeque is: ";
    printDeque(myDeque);                    // 输出: 10000 1 2 3 4 5
    cout << "After insert to the front, the first element is: " << *it_first << endl;  // 输出1, 迭代器不正确(失效)
    cout << "Before insert to the front, the last element is: " << *it_last << endl;   // 输出5, 迭代器指向正确元素

    // insert a new element to the end
    myDeque.push_back(10001);
    cout << "After insert, myDeque is: ";
    printDeque(myDeque);                    // 输出: 10000 1 2 3 4 5 10001
    cout << "After insert to the end, the first element is: " << *it_first << endl;  // 输出1, 仍然是失效的
    cout << "Before insert to the end, the last element is: " << *it_last << endl;   // 输出5, 迭代器失效

    // 重新获取一次迭代器
    it_first = myDeque.begin();
    it_last = myDeque.end() -1;
    cout << "-----------" << endl;
    cout << "After update, the first element is: " << *it_first << endl;  // 输出10000, 有效
    cout << "Before update, the last element is: " << *it_last << endl;   // 输出10001, 有效

    // insert to middle position of myDeque
    myDeque.insert(it_first+2, 0);
    cout << "-----------" << endl;
    cout << "After insert 0 to middle of myDeque: ";
    printDeque(myDeque);

    cout << "After insert to some middle position, the first element is: " << *it_first << endl;  // 输出1, 仍然是失效的
    cout << "Before insert to some middle position, the last element is: " << *it_last << endl;   // 输出5, 迭代器指向元素正确

    return 0;
}