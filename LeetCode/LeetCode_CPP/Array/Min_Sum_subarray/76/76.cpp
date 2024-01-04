#include <iostream>
using namespace std;

class Solution
{
    public:
        string minWindow(string s, string t) 
        {
            int cnt = 0;                            // 统计有效字符(t里包含的字符)
            int minLength = INT_MAX;
            string res = "";

            // 构建 t 的哈希表
            unordered_map<char, int> t_hash;
            for(char c : t){
                t_hash[c]++;
            }

            // 滑动窗口
            unordered_map<char, int> win_hash;      // 窗口内所包含字符构成的哈希表
            for(int left = 0, right = 0; right < s.size(); right++){
                win_hash[s[right]]++;               // 窗口放入新元素, 更新窗口哈希表

                // 统计有效字符的个数 (t中不包含该字符时 t_hash[s[i]] 为 0)
                if(win_hash[s[right]] <= t_hash[s[right]]){
                    cnt++;
                }

                // 当窗口中包含t中所有字符种类时, 收缩窗口左边界
                if(cnt == t.size()){
                    while(win_hash[s[left]] > t_hash[s[left]]){
                        win_hash[s[left]] --;
                        left++;
                    }

                    // 判断当前窗口内字符是否更短
                    if(right - left + 1 < minLength){
                        res = s.substr(left, right - left + 1);
                        minLength = res.size();
                    }
                }
            }

            return res;
        }
};

int main(void)
{
    string s = "ADOBECODEBANC";
    string t = "ABC";
    Solution res;

    cout << res.minWindow(s,t) << endl;

}