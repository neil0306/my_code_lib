#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

// case 1: 测试 join 的功能
void run_1(int x)
{
    while (x --> 0){
        cout << "CppNuts." << endl;
    }

    std::this_thread::sleep_for(chrono::seconds(5));  // 停5秒
    cout << "after thread sleep" << endl;
}

// case 2: 测试 detach 的功能
void run_2(int x)
{
    while (x --> 0){
        cout << x << " CppNuts." << endl;
    }
    cout << "thread finished" << endl;
}

int main(void)
{

    // -------------- case 1: 测试 join ------------------
    // std::thread t1(run_1, 10);   // 创建线程之后, run函数会马上开始执行, 并立即返回到main这里, 并继续往下执行main的语句
    // cout << "main()" << endl;

    // // 执行某些操作让主线程停一会儿
    // std::this_thread::sleep_for(chrono::seconds(2));  // 停2秒
    // cout << "test 1111" << endl;  // 可以观察到: 这里的输出是在run在后台执行的过程中打印出来的, 说明run在后台并行执行中

    // t1.join();                    // 通过输出可以观察到: 下面的“main() after” 会在run执行结束后才输出, 因此join功能确实是等待指定线程结束运行. 

    // if(t1.joinable()){
    //     t1.join();                // 我们不能直接连续使用两次join(), 因为执行完一次join就已经说明线程执行完毕, 所以这里用了 joinable() 来做一次检查, 防止join连续用了两次
    // }

    // cout << "main() after" << endl;
    // -----------------------------------------------------


    // case 2: 测试detach
    std::thread t2(run_2, 10);
    cout << "main()" << endl;

    t2.detach();                // 使用detach(), 意味着它的父线程不会等待这个子线程执行完, 而是自顾自地执行父线程, 当父线程执行完时, 不管子线程被执行到什么位置, 子线程都会马上被系统终止.
    if(t2.joinable()){          // 同样, 防止重复使用detach也要先检查一下, 并且检查时使用的是 joinable() 
        t2.detach();
    }
    cout << "main() after" << endl;
    // std::this_thread::sleep_for(chrono::seconds(5));  // 停5秒, 观察使用了detach的线程有没有执行完

    return 0;
}

