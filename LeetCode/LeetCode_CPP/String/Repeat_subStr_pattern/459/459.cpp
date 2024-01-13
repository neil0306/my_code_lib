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
            int len = s.size();
            int next[s.size()];
            getNext(next, s);  // 遍历了一次原数组, 时间复杂度 O(m+n)

            if(next[len-1] != 0 && len % (len - next[len-1]) == 0){
                    return true;
            }
            return false;
        }

        // 前缀表直接作为Next数组
        void getNext(int * next, string s)
        {
            // 初始化
            int j = 0;
            next[0] = 0;
            
            for(int i = 1; i < s.size(); i++){
                // 处理前后缀元素不相等
                while(j > 0 && s[i] != s[j]){
                    j = next[j-1];
                }
                // 处理前后缀元素相等
                if(s[i] == s[j]){
                    j++;
                }
                next[i] = j;
            }
        }

        // 前缀表-1作为next数组
        void getNext_sub1(int * next, string s)
        {
            int j = -1;
            next[0] = j;

            for(int i = 1; i < s.size(); ++i){
                while(j >= 0 && s[i] != s[j+1]){
                    j = next[j];
                }
                if(s[i] == s[j+1]){
                    j++;
                }
                next[i] = j;
            }
        }

        bool repeatSubstringPattern_KMP_sub1(string s)
        {
            int len = s.size();
            int next[s.size()];
            getNext_sub1(next, s);  // 遍历了一次原数组, 时间复杂度 O(m+n)

            if(next[len-1] != -1 && len % (len - (next[len-1]+1)) == 0){
                    return true;
            }
            return false;
        }
};

int main(void)
{
    Solution ans;
    string s;

    s = "ababababababa";
    s = "ababc";
    cout << (ans.repeatSubstringPattern(s)== true ? "true" : "false") << endl;
    cout << (ans.repeatSubstringPattern_KMP(s)== true ? "true" : "false") << endl;
    cout << (ans.repeatSubstringPattern_KMP_sub1(s)== true ? "true" : "false") << endl;
    return 0;
}