# QUESTIONS
1. What do you understand by thread and give one example in C++?

## ANSWER
0. In every application there is a default thread which is `main()`, in side this we create other threads.
1. A thread is also known as lightweight process. Idea is achieve parallelism by dividing a process into multiple threads. 
   For example:
   (a) The browser has multiple tabs that can be different threads. 
   (b) MS Word must be using multiple threads, one thread to format the text, another thread to process inputs (spell checker)
   (c) Visual Studio code editor would be using threading for auto completing the code. (Intellicence)

WAYS TO CREATE THREADS IN C++11
1. Function Pointers
2. Lambda Functions
3. Functors
4. Member Functions
5. Static Member functions

## 要点
- C++11 标准之后才引入 thread, 在C++11之前都没有这个功能
- thread 必须依附 main 才能执行, 因此通常需要在 main 或者 main里调用的函数中开启线程, 通常会使用 join() 等待线程执行结束, 如果不等待, 则 main 函数的流程一旦运行完毕, 无论后台的线程是否结束都会被系统强制结束掉.


在编译使用了多线程的C++代码时, 需要指明C++标准的版本`c++11`, 以及使用 `pthread`库, 比如:
> g++ -std=c++11 -pthread   test.cpp 

# Reference
- https://www.youtube.com/watch?v=TPVH_coGAQs&list=PLk6CEY9XxSIAeK-EAh3hB4fgNvYkYmghp&index=1
