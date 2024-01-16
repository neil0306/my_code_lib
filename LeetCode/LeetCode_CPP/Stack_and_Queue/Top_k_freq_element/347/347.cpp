#include <iostream>
#include <queue>            // 为了使用 priotiry_queue

using namespace std;

class Solution
{
    public:
        // 创建优先级队列时, 需要自定义比较时使用的 "函数符"
        class myComp
        {
            public:
                /*
                    1. 由于优先级队列中输入的是 key-value 的数据, 故比较的时候也要接收这种类型的数据
                        使用 pair<type1, type2> 就可以实现这种结构

                    2. 这里定义的是仿函数, 或者叫函数对象, 它可以嵌套STL里的函数模板, 从而避免了提前定义参数类型(虽然这里也还是指定了类型..)
                */
                bool operator()(const pair<int, int> & lhs, const pair<int, int> &rhs){
                    return lhs.second > rhs.second;
                }
        };

    public:
        vector<int> topKFrequent(vector<int>& nums, int k) 
        {
            unordered_map<int, int> freqMap;

            // step1: 统计元素出现的频率
            for(int i = 0; i < nums.size(); ++i){
                freqMap[nums[i]]++;
            }

            // step2: 按照频率排序
            // 使用优先级队列(内部有堆的实现, 我们只需要用自定义的函数符来指定大顶堆还是小顶堆即可)
            priority_queue< pair<int,int>, vector<pair<int,int>>, myComp> pri_que;

            // 遍历频率哈希表, 利用优先级队列进行排队
            for(unordered_map<int, int>::iterator it = freqMap.begin(); it != freqMap.end(); it++){
                pri_que.push(*it);
                if(pri_que.size() > k){     // 最多只能放k个元素, 相当于限制了大小堆的容量
                    pri_que.pop();
                }
            }

            // 拿出结果
            vector<int> res(k);
            for(int i = k-1; i >=0; i--){
                res[i] = pri_que.top().first;
                pri_que.pop();
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

    nums = {1,1,1,2,2,3}, k = 2;
    res = ans.topKFrequent(nums, k);

    for(auto it:res){
        cout << it << " ";
    }
    cout << endl;
    return 0;
}