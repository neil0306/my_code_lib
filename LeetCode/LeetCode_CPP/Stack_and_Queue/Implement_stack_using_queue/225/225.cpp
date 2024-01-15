#include <iostream>
#include <queue>

using namespace std;

class MyStack {
    public:
        queue<int> myQueue;
        MyStack() {
        }
        
        void push(int x) {
            myQueue.push(x);        // 直接将元素放入队列尾部
        }
        
        int pop() {
            int size = myQueue.size()-1;
            // 只循环 size-1 次.
            while(size > 0){
                myQueue.push(myQueue.front());      // front负责取当前队列头部元素, push负责放入队列尾部
                myQueue.pop();                      // 放完之后, 队列头的元素就重复了, 丢掉
                size--;
            }
            int result = myQueue.front();             // 此时取到的就是原来队列的最后一个元素
            myQueue.pop();
            return result;
        }
        
        int top() {
            return myQueue.back();                      // back负责读取队列尾部元素 (但是没有直接弹出它)
        }
        
        bool empty() {
            return myQueue.empty();
        }
};


int main(void)
{
    return 0;
}