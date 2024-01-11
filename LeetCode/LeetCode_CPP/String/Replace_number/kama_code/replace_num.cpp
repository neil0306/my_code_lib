#include <iostream>
using namespace std;

class Solution
{
    public:
        string replaceNumber(string &s)
        {
            int cnt_num = 0;
            int ori_size = s.size();

            // 统计数字个数
            for(int i = 0; i < ori_size; i++){
                if('0' <= s[i] && s[i] <= '9'){
                    cnt_num++;
                }
            }

            // 调整大小
            s.resize(ori_size + cnt_num * 5);   // 虽然 "number" 有6个字母, 但是我们替换的位置本身就有1个空位, 所以只需要扩大5倍
            int new_size = s.size();

            // 填充
            for(int i = ori_size-1, j =  new_size-1; j > i; i--, j--){
                if(s[i] < '0' || s[i] > '9'){   // 字符
                    s[j] = s[i];
                }
                else{                           // 数字
                    s[j] = 'r';
                    s[j - 1] = 'e';
                    s[j - 2] = 'b';
                    s[j - 3] = 'm';
                    s[j - 4] = 'u';
                    s[j - 5] = 'n';
                    j -= 5;
                }
            }
            return s;
        }
};

int main(void)
{
    Solution ans;
    string s;

    s = "a1b2c3";
    cout << ans.replaceNumber(s) << endl;
    return 0;
}