#include <iostream>
using namespace std;

class Solution
{
    public:
        string right_turn_str(string & s, int k)
        {
            reverse_str(s, 0, s.size()-1);
            reverse_str(s, 0, k-1);
            reverse_str(s, k, s.size()-1);
            return s;
        }

        void reverse_str(string & s, int start, int end)   // 区间[start, end]
        {
            for(int i = start, j = end; i < j; ++i, --j){
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
    s = "a", k = 1;
    cout << ans.right_turn_str(s,k) << endl;

    return 0;
}