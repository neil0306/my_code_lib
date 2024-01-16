#include <iostream>
#include <deque>

using namespace std;

class Solution
{
    public:
        class MonoQue
        {
            public:
                deque<int> mono_que;        // 单调队列

                void pop(int val)
                {
                    if(mono_que.empty() == false && mono_que.front() == val){   // 若要弹出的元素是队列头
                        mono_que.pop_front();       // 弹出队列头部元素
                    }
                }

                void push(int val)
                {
                    // 清空队列
                    while(mono_que.empty() == false && val > mono_que.back()){
                        mono_que.pop_back();        // 注意, 要用尾部逐一弹出, 并再次判断, 防止把最大的元素弹出
                    }
                    // 将新元素加入队列尾
                    mono_que.push_back(val);
                }

                int front()
                {
                    return mono_que.front();
                }
        };

    public:
        vector<int> maxSlidingWindow(vector<int>& nums, int k) 
        {
            vector<int> res;
            MonoQue que;

            // 处理第一个窗口
            for(int i = 0; i < k; ++i){
                que.push(nums[i]);
            }
            res.push_back(que.front());

            // 滑动窗口
            for(int i = k; i < nums.size(); ++i){
                que.pop(nums[i-k]);     // 处理要移出窗口的元素
                que.push(nums[i]);      // 处理新加入窗口的元素
                res.push_back(que.front()); // 获得当前窗口最大值
            }
            return res;
        }
};

int main(void)
{
    vector<int> nums;
    int k;
    Solution ans;
    vector<int> res;

    nums = {1,3,-1,-3,5,3,6,7}, k = 3;
    nums = {1}, k = 1;
    nums = {1,3,1,2,0,5}, k = 3;
    
    res = ans.maxSlidingWindow(nums, k);

    for(int i : res){
        cout << i << " ";
    }
    cout << endl;
    return 0;
}


