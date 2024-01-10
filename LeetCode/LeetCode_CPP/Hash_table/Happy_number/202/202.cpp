#include <iostream>
#include <unordered_set>

using namespace std;

class Solution
{
    public:
        // 暴力解法
        bool isHappy1(int n)
        {
            if(n == 1){
                return true;
            }

            int index = 0;
            int sum = 0;
            while(index < 810)
            {
                while(n > 0){
                    sum += (n%10) * (n%10);     // 每一位的平方
                    n /= 10;                    // 当前数字的个位已经处理完, 去掉
                }

                // 求和完, 看看是不是快乐数
                if(sum == 1){
                    return true;
                }

                // 为下一次循环更新
                n = sum;
                sum = 0;
                index++;
            }
            return false;
        }

        // 哈希表法
        bool isHappy2(int n)
        {
            if(n == 1){
                return true;
            }
            
            unordered_set<int> sum_set;
            while(true)
            {
                
                // 对n的每一位数字求平方和
                int sum = 0;
                while(n > 0){
                    sum += (n%10) * (n%10);
                    n /= 10;
                }
                n = sum;
                
                // 检查sum是否满足快乐数条件
                if(sum == 1){
                    return true;
                }

                // 检查是否有必要继续循环
                if(sum_set.find(sum) != sum_set.end()){     // 曾经出现过这个值
                    return false;
                }
                else{
                    sum_set.insert(sum);
                }
            }
        }
};

int main(void)
{
    int n;
    Solution ans;

    n = 2;

    cout << n << " is Happy number? | ans = " << (ans.isHappy2(n) == true ? "true" : "false") << endl;
    return 0;
}