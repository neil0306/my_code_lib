#include <iostream>
using namespace std;

/*
// ------ 彩蛋? ---------
    #define a o_o
    #define b O_q
    #define main a_a
    typedef bool O_o;
    typedef void o_q;
    o_q o_o(O_o){}

    o_q a_a(o_q){
        O_o O_q;    
        o_o(O_q);
    }
*/


class Solution
{
    public:
        // 简单版: 使用了O(n)的额外空间
        string reverseWords(string s)
        {
            string res = "";
            vector<string> temp_res;

            for(int right = 0, left = 0; left <= right && right < s.size();){
                // 右侧指针找到空格
                while(s[right] != ' ' && right < s.size()){
                    right++;
                }
                // 左侧指针找非空格
                while(left <= right && s[left] == ' '){
                    left++;
                }
                // 有效字符, 放入结果中
                if(right - left > 0){
                    string temp = s.substr(left, right-left);
                    temp_res.push_back(s.substr(left, right-left));
                }
                left = right++;     // 重置left
                left++;
            }

            if(temp_res.size() > 0){
                for(std::vector<string>::iterator it=temp_res.end()-1; it >= temp_res.begin(); it--){
                    res += *it;
                    if(it != temp_res.begin()){
                        res += " ";
                    }
                }
            }
            return res;
        }

        // 升级版: 只能使用 O(1) 的空间  => 反转两次字符串
        string reverseWords_opt(string s)
        {
            // ------ 双指针去空格: 去掉所有空格, 并手动在单词之间补一个空格 -------
            int slow = 0;                       // 指向单词
            for(int fast = 0; fast < s.size(); ++fast){
                if(s[fast] != ' '){             // 遇到非空格的时候才操作元素, 遇到空格就跳过
                    // 针对字符串中间的单词手动补空格, 补完之后更新slow
                    if(slow != 0){
                        s[slow++] = ' ';
                    }
                    // 对于单词部分, 将单词复制到slow所在位置
                    while(fast < s.size() && s[fast] != ' '){
                        s[slow++] = s[fast++];
                    }
                }
            }
            s.resize(slow);     // 去掉冗余空格后的字符串

            // --------- 第一次翻转: 反转整个字符串 ------
            reverse_str(s, 0, s.size()-1);

            // --------- 第二次翻转: 反转单词 ------
            slow = 0;
            for(int i = 0; i <= s.size(); ++i){
                if(s[i] == ' ' || i == s.size()){
                    reverse_str(s, slow, i-1);
                    slow = i+1;
                }
            }
            return s;
        }

        void reverse_str(string & s, int start, int end){
            for(int i = start, j = end; i < j; ++i, --j){
                swap(s[i], s[j]);
            }
        }
};

int main(void)
{
    string s;
    Solution ans;

    s = " ";
    s = " the sky   is blue  ";
    // s = s = "  hello world  ";
    // s = s = "a good   example";
    cout << "|"<< ans.reverseWords_opt(s) << "|" << endl;
    return 0;
}