#include <iostream>

using namespace std;

class Solution
{
    public:
        int MySqrt(int x)
        {
            // 使用二分法: 使用左闭右闭区间, x 相当于target, 但比较大小的时候是 x 与 mid * mid 比较
            int max = x;
            int min = 0;
            
            while(min <= max){
                int mid = min + (max - min) / 2;
                if(x / mid < mid){      // x < mid * mid
                    max = mid -1;
                }
                else if(x / mid > mid){ // x > mid * mid
                    min = mid + 1;
                }
                else{
                    return mid;
                }
            }
            return max;
        }
};

int main(void)
{
    Solution s;

    int x = 4;
    cout << "x = " << x << " | " << "sqrt(x) = " << s.MySqrt(x) << endl;
    
    x = 8;
    cout << "x = " << x << " | " << "sqrt(x) = " << s.MySqrt(x) << endl;

    x = 9;
    cout << "x = " << x << " | " << "sqrt(x) = " << s.MySqrt(x) << endl;

    x = pow(2,31) -1;
    cout << "x = " << x << " | " << "sqrt(x) = " << s.MySqrt(x) << endl;

    return 0;
}