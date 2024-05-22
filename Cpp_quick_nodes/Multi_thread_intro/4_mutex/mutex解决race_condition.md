# Mutex: Mutual Exclusion (互斥)

## RACE CONDITION:
0. Race condition is a situation where two or more threads/process happens to change a common data at the same time.
1. If there is a race condition then we have to protect it and the protected section is called critical section/region.

## MUTEX:
0. Mutex is used to avoid race condition.
1. We use lock() , unlock() on mutex to avoid race condition.


啥时候用mutex?
- 当程序中存在可能出现`race condition(竞争冒险)`的时候, 就必须使用 mutex 使线程访问数据的时候互斥

为什么要用mutex?
- 当出现 race condition 时, 如果不使用mutex, 就可能出现计算错误, 比如`++i`这样简单的操作, 假设i初始化为0, 如果不使用mutex, 则如果开了100个线程, 其中就可能有30线程同时读到了初始化的0值, 这些线程自增后得到的结果都是1, 然后这30个线程还可能会在不同时间节点给i写入1, 这个写入操作就会刷掉其他线程的计算结果, 使得i永远不会是我们想要的正确结果: 100. 

使用mutex是怎么实现保护的?
- 当每一个线程中使用 lock 和 unlock 进行保护后, 线程i在执行操作的过程中, 另外的线程(比如j线程)将需要等待线程i完成计算并unlock之后才可以进行相应的操作.
  - 打个比方就是上厕所, 有一群人想抢着上, 但是只有1把钥匙可以开厕所门, 于是第一个抢到钥匙的人将可以进厕所并锁住厕所门, 当这个人用完厕所, 解锁了厕所门之后, 其他人才能继续抢钥匙并上厕所.