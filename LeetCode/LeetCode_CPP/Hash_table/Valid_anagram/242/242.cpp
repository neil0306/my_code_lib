#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

class Solution
{
    public:
        bool isAnagram1(string s, string t) {
            if (s.size() != t.size()) return false;
            unordered_map<char, int> m1, m2;
            for (int i = 0; i < s.size(); ++i) {
                ++m1[s[i]];
                ++m2[t[i]];
            }
            return m1 == m2;
        }

        bool isAnagram2(string s, string t)
        {
            if(s.size() != t.size()){
                return false;
            }

            int record[26] = {0};       // 用来统计 26 个字母出现的次数, 并且用ASCII的相对数值来区分不用字母

            // 统计 s 中每个字母出现的次数
            for(int i = 0; i < s.size(); ++i){
                record[s[i] - 'a']++;
                record[t[i] - 'a']--;
            }

            // 检查record数组, 如果里面有不为零0的元素, 证明两个字符串的字母数量并不完全相等
            for(int i = 0; i < 26; i++){
                if(record[i] != 0){
                    return false;
                }
            }
            return true;
        }
};



int main(void)
{
    string s, t;
    Solution ans;

    s = "";
    t = "";

    cout << (ans.isAnagram1(s, t) == true ? "true" : "false") << endl;
    cout << (ans.isAnagram2(s, t) == true ? "true" : "false") << endl;

    return 0;
}