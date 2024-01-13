#include <iostream>

using namespace std;

class Solution
{
    public:
        int strStr(string haystack, string needle)
        {
            // needle 为空, 返回0
            if(needle.size() == 0){
                return 0;
            }

            int len_n = needle.size();
            int len_h = haystack.size();

            if(len_n <= len_h){
                for(int i = 0; i < haystack.size(); i++){
                    // 剩余长度不足, 肯定匹配不上, 直接返回
                    if(len_h - i < len_n){
                        return -1;
                    }

                    // 看看能不能匹配上
                    for(int j = 0; j < len_n; j++){
                        if(haystack[i+j] != needle[j]){
                            break;
                        }
                        else if(j == len_n -1){
                            return i;
                        }
                    }
                }
            }
            return -1;
        }

        // -------------- KMP 算法 -----------------
        vector<int> getNext(string s)
        {
            // step1 初始化
            int j = 0;      
            vector<int> Next{0};

            // 子串逐渐变长
            for(int i = 1; i < s.size(); ++i){
                // step2 处理前后缀不相等
                while(j > 0 && s[i] != s[j]){       // 后缀指针i一直指向 "最长后缀" 的最后一个元素
                    j = Next[j-1];                  // 前后缀末尾元素不相等时, 缩短前缀, j指向 "最长相等前后缀" 的前缀最后一个元素
                }

                // step3 处理前后缀相等
                if(s[i] == s[j]){
                    j++;
                }

                // 填充Next数组
                Next[i] = j;
            }
            return Next;
        }

        int strStr_KMP(string haystack, string needle)
        {
            vector<int> Next = getNext(needle);
            int j = 0;
            for(int i = 0; i < haystack.size(); ++i){
                while(j > 0 && haystack[i] != needle[j]){       // 可能需要多次回退, 所以要用while!!!
                    j = Next[j-1];
                }
                if(haystack[i] == needle[j]){
                    j++;
                }
                if(j == needle.size()){
                    return (i - needle.size() + 1); 
                }
            }
            return -1;
        }
};


int main(void)
{
    string haystack, needle;
    Solution ans;

    haystack = "sadbutsad", needle = "sad";
    haystack = "leetcode", needle = "leeto";
    haystack = "hello", needle = "ll";
    cout <<  ans.strStr(haystack, needle) << endl;
    cout <<  ans.strStr_KMP(haystack, needle) << endl;
    return 0;
}