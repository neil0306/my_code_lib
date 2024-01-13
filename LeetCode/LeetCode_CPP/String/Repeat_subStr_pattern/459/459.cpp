#include <iostream>
using namespace std;

class Solution
{
    public:
        // 移动匹配法: 时间复杂度 O(m+n), 由find()方法引入
        bool repeatSubstringPattern(string s)
        {
            return ((s+s).substr(1, s.size()*2 -2)).find(s) != string::npos;
            // 完整版:
            // string  t = (s+s).substr(1, s.size()*2 -2);     // 掐头去尾
            // if(t.find(s) != string::npos){
            //     return true;
            // }
            // return false;
        }

        bool repeatSubstringPattern_KMP(string s)
        {
            
        }
};

int main(void)
{
    Solution ans;
    string s;

    s = "aa";
    cout << (ans.repeatSubstringPattern(s)== true ? "true" : "false") << endl;
    return 0;
}