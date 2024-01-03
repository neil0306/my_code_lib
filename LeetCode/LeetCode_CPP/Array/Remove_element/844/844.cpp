#include <iostream>

using namespace std;

class Solution {
public:
    bool backspaceCompare(string s, string t) {
        int i = s.length() -1, j = t.length() -1;   // 双指针, 从末尾开始遍历
        int cnt_s = 0, cnt_t = 0;                           // 记录某个区间内#号个数 

        while(true){

            // 干掉s里的井号, 注意处理 "a#c#" 这种情况, 井号不连续出现, 但是处理的时候其实需要连续退格的
            while(i >= 0){
                if(s[i] == '#'){
                    cnt_s++;
                }
                else{
                    // 处理退格
                    if(cnt_s > 0){
                        cnt_s--;    // 每次循环退格一次
                    }
                    else{
                        break;      // 当前区域退格完成, 跳出这层while
                    }
                }
                i--;
            }

            // 处理t字符串的井号
            while(j >= 0){
                if(t[j] == '#'){
                    cnt_t++;
                }
                else{
                    // 处理井号的退格
                    if(cnt_t > 0){
                        cnt_t--;    // 每次循环退格一次
                    }
                    else{
                        break;      // 当前区域退格完成, 跳出这层while
                    }
                }
                j--;
            }
        
            // 如果其中一个字符已经遍历到字符串开头, 跳出整个循环
            if(i < 0 || j < 0){
                break;
            }

            // 处理完退格, 开始比较字符串(此时比较的是末尾的有效字符), 只要有一个字符不同, 就结束
            if(s[i] != t[j]){
                return false;
            }

            // 走到这里, 证明进行了退格之后的第一个有效字符是相等的, 继续往字符串开头方向更新
            i--;
            j--;
        }

        if(j == -1 && i == -1){  // 同时遍历完, 说明相等
            return true;
        }

        return false;           // 其中一个字符串提前遍历结束时, 返回false
    }
};

int main(void)
{
    string s = "ab#c";
    string t = "ad#c";
    Solution res;

    cout << (res.backspaceCompare(s,t) == true ? "True" :"False") << endl;
    return 0;
}