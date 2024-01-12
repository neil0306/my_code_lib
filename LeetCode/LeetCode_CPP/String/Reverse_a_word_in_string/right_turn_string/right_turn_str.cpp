#include <iostream>
using namespace std;

class Solution
{
    public:
        string right_turn_str(string s, int k)
        {
            reverse_str(s, 0, s.size()-1);      // 先整体翻转
            reverse_str(s, 0, k-1);             // 反转[0,k-1]
            reverse_str(s, k, s.size()-1);      // 反转[k,s.size()-1]
            return s;
        }

        void reverse_str(string & s, int start, int end)   // 区间[start, end]
        {
            for(int i = start, j = end; i < j; ++i, --j){
                swap(s[i], s[j]);
            }
        }

        string left_turn_str(string s, int k)
        {
            reverse_str(s, 0, k-1);        // 反转[0,k-1]
            reverse_str(s, k, s.size()-1); // 反转[k,s.size()-1]
            reverse_str(s, 0, s.size()-1); // 反转整个字符串
            return s;
        }
};

int main(void)
{
    int k;
    string s;
    Solution ans;

    s = "a", k = 1;
    s = "abcdefg", k = 2;
    cout << ans.right_turn_str(s,k) << endl;        // fgabcde
    cout << ans.left_turn_str(s,k) << endl;         // cdefgab

    return 0;
}