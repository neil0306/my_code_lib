#include <iostream>
#include <stack>            // 不加头文件的话, 会找不到stack模板
using namespace std;

class MyQueue {
    public:
        stack<int> stackIn;
        stack<int> stackOut; 
        MyQueue() {
        }
        
        void push(int x) {
            stackIn.push(x);
        }
        
        int pop() {
            if(stackOut.empty()== true){              // 如果 stackOut 里有元素, 此时直接输出 stackOut 的栈顶即可
                while(stackIn.empty() == false){
                    stackOut.push(stackIn.top());     // 用top取出元素, 因为pop没有返回值
                    stackIn.pop();
                }
            }

            int result = stackOut.top();
            stackOut.pop();
            return result;
        }
        
        int peek() {
            int result = this->pop();  // 复用写好的方法
            stackOut.push(result);
            return result;
        }
        
        bool empty() {
            return stackOut.empty() && stackIn.empty();
        }
};


int main(void)
{
    return 0;
}