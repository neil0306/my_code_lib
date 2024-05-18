#include <iostream>
#include <thread>

using namespace std;

// case 1: 直接用 function pointer 传入thread
void fun(int x)
{
    while(x-- > 0){
        cout << x << endl;
    }
}

// case 3: 非静态成员函数(non-static member function)
class Case_3
{
public:
    void operator()(int x)   // 只要重载了“()”运算符, 这个类就具有了仿函数的功能
    {
        while (x-- > 0){
            cout << x << endl;
        }
    }
};

// case 4: Non-static member function
class Case_4
{
public:
    void run(int x)
    {
        while (x-- > 0){
            cout << x << endl;
        }
    }
};

// case 5: static member function
class Case_5
{
public:
    static void run(int x)          // 加了static关键词, 表明是一个静态成员函数
    {
        while (x-- > 0){
            cout << x << endl;
        }
    }
};

int main(void)
{
    // case 1: function pointer
    // std::thread t1(fun, 10);
    // t1.join();


    // case 2: Lambda function
    // std::thread t2([](int x){           // lambda 函数
    //     while(x-- > 0){
    //         cout << x << endl;
    //     }
    // }, 10);
    // t2.join();


    // case 3: functor (Function Object, 仿函数)
    // std::thread t3((Case_3()), 10);     // 注意在传入 Case_3() 的时候外面还套了一层括号 
    // t3.join();


    // case 4: non-static member function
    // Case_4 c4;
    // std::thread t4(&Case_4::run, &c4, 10);   // 由于是非静态成员函数, 它必须依赖一个对象才能调用, 因此这里还传入了一个对象的地址
    // t4.join();


    // case 5: static member function
    std::thread t5(&Case_5::run, 10);
    t5.join();

    return 0;
}

