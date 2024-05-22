#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int counter = 0;        // 全局变量, 存放在内存的“静态存储区”
std::mutex mtx;         // 全局对象

void increaseTheCounterFor100000Time()
{
    for (int i = 0; i < 100000; ++i){
        if(mtx.try_lock()){
            ++counter;
            mtx.unlock();
        }
    }
}

int main(void)
{
    // 开两个线程
    std::thread t1(increaseTheCounterFor100000Time); 
    std::thread t2(increaseTheCounterFor100000Time);

    if(t1.joinable()){
        t1.join();
    }
    if(t2.joinable()){
        t2.join();
    }

    cout << "counter could increase upto: " << counter << endl;  
    /*
        由于try_lock会立即返回, 这里输出的counter不太可能是会是200000.
            因为 t1 如果 lock 成功, t2 的if语句就会马上返回导致for循环继续执行. 
            因此就会导致某些时刻没有执行count++.
    */ 
    return 0;
}