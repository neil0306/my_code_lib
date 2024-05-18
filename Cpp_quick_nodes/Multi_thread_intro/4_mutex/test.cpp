#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

int myAccount = 0;  // 全局变量
std::mutex m;       // mutex 对象m

void addMoney()
{
    m.lock();
    ++myAccount;    // ++操作要走三个步骤: (1)读取变量的值; (2)增加1; (3)存储增加后的值    ==> 这一步在多线程里会触发竞争(race condition)
    m.unlock();
}

int main(void)
{
    std::thread t1(addMoney);
    std::thread t2(addMoney);

    if(t1.joinable())
        t1.join();
    if(t2.joinable())
        t2.join();
    
    cout << myAccount << endl;      // 如果不使用 mutex 的lock, 多线程的加法就很可能给出错误的计算结果.
    return 0;
}