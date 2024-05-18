# JOIN NOTES
0. Once a thread is started we wait for this thread to finish by calling join() function on thread object.
1. Double join will result into program termination.
2. If needed we should check thread is joinable before joining. ( using joinable() function)

# DETACH NOTES
0. This is used to detach newly created thread from the parent thread.
1. Always check before detaching a thread that it is joinable otherwise we may end up double detaching and 
   double detach() will result into program termination.
2. If we have detached thread and main function is returning then the detached thread execution is suspended.

NOTES:
Either join() or detach() should be called on thread object, otherwise during thread object�s destructor it will 
terminate the program. Because inside destructor it checks if thread is still joinable? if yes then it terminates the program.
> 如果创建了一个线程, 那么后面的程序一定要有对应的 join() 或者 detach()!!! 
> 在使用join和detach时, 一定要养成先检查 joinable() 的习惯! 
- 这是因为当一个线程被创建时, 系统会自动把这个线程标记为 joinable, 而一旦这个线程用过一次 join() 或者 detach(), 这个线程就会被标记为 non-joinable.
