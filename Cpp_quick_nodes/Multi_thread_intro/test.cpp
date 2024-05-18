/*
task: 从给定的数字范围内求出“奇数的和” 以及 “偶数的和”.
*/

#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
using namespace std;
using namespace std::chrono;
typedef long long int  ull;

// 偶数和
void findEven(ull start, ull end, ull* EvenSum) {
	for (ull i = start; i <= end; ++i){
		if (!(i & 1)){
			*(EvenSum) += i;
		}
	}
}

// 奇数和
void findOdd(ull start, ull end, ull* OddSum) {
	for (ull i = start; i <= end; ++i){
		if (i & 1){
			(*OddSum) += i;
		}
	}
}

int main() {
	
	ull start = 0, end = 1900000000;

	ull OddSum = 0;
	ull EvenSum = 0;
    
    auto startTime = high_resolution_clock::now();   // 计时器

	// // WITH THREAD
    std::thread t1(findEven, start, end, &(EvenSum));  // 创建线程
    std::thread t2(findOdd, start, end, &(OddSum));    // 创建线程

    /* 使用 join() 等待线程执行结束, 如果不等待, 则 main 函数的流程一旦运行完毕, 无论后台的线程是否结束都会被系统强制结束掉 */
	t1.join();      // main线程走到这里时, 遇到join表示需要等待 t1 执行结束才能继续往下运行
	t2.join();      // main线程走到这里时, 遇到join表示需要等待 t2 执行结束才能继续往下运行

	// // WITHOUT THREAD
	// findEven(start,end, &EvenSum);
	// findOdd(start, end, &OddSum);
    auto stopTime = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stopTime - startTime);

	cout << "OddSum : " << OddSum << endl;
	cout << "EvenSum : " << EvenSum << endl;

	cout << "Sec: " << duration.count()/1000000 << endl;

	return 0;
}