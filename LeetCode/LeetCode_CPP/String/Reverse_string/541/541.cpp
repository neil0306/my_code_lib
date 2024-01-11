#include <iostream>

using namespace std;

class Solution 
{
    public:
        string reverseStr(string s, int k) 
        {
            int i = 0;
            int n = s.size();
            while(i < n){
                if(i + k > n) {            // 剩余字符不足k个
                    reverse_1k(s, i, n);
                }
                else{
                    reverse_1k(s, i, i+k);
                }
                i += 2*k;
            }
            return s;
        }

        // 传入区间为左闭右开: [start, end)
        void reverse_1k(string &s, int start, int end)
        {
            for(int i = start, j = end-1; i < j; i++, j--){
                swap(s[i], s[j]);
            }
        }
};

int main(void)
{
    int k;
    string s;
    Solution ans;

    s = "abcdefg", k = 2;
    s = "abcde", k = 2;
    cout << ans.reverseStr(s,k) << endl;
    return 0;
}