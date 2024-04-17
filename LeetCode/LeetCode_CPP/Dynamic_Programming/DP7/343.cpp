#include <iostream>
#include <vector>

using namespace std;

class Solution
{
public:
    int integerBreak(int n) {
        vector<int> dp = vector<int>(n+1, 0);  // dp[i] 表示将 i 拆分后得到的最大乘积, 因为n是要拆的目标，所以数组长度得是n+1
        
        dp[0] = 0;  // 不初始化dp[0] 和 dp[1]也可以
        dp[1] = 0;
        dp[2] = 1;

        for (int i = 3; i <= n; ++i){      // 分析递推过程可以发现，不需要遍历到n，遍历到 n/2 就够了，后面的值不会大于前 n/2 的结果
            for (int j = 1; j <= i/2; ++j){
                int history = dp[i];         // 为了方便理解，单独写出来
                dp[i] = max( history, max(j * (i-j), j * dp[i-j]) );  // 主要不要漏掉与【i的历史拆分结果】进行比较
            }
        }

        return dp[n];
    }
};

int main(void)
{
    return 0;
}