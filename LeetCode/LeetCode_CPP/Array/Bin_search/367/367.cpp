#include <iostream>

using namespace std;

class Solution {
    public:
        bool isPerfectSquare(int num) 
        {
            int min = 0.0;
            int max = num;

            while(min <= max){
                int mid = min + (max - min) / 2;
                if(num / mid < mid){ // num < mid * mid 
                    max = mid - 1;
                }
                else if (num / mid > mid) { // num > mid * mid
                    min = mid + 1;
                }
                else{
                    return true;
                }
                // cout << "min = " << min << "|" << "max = " << max << endl;
            }
            return false;
        }
};

int main(void)
{
    int num;
    bool res;
    Solution s;

    num = 16;
    res = s.isPerfectSquare(num);
    cout << num << ": " << (res == true ? "True" : "False") << endl;

    num = 14;
    res = s.isPerfectSquare(num);
    cout << num << ": " << (res == true ? "True" : "False") << endl;


    num = 1;
    res = s.isPerfectSquare(num);
    cout << num << ": " << (res == true ? "True" : "False") << endl;

    num = 0;
    res = s.isPerfectSquare(num);
    cout << num << ": " << (res == true ? "True" : "False") << endl;

    num = pow(2, 31) -1;
    res = s.isPerfectSquare(num);
    cout << num << ": " << (res == true ? "True" : "False") << endl;
    return 0;
}