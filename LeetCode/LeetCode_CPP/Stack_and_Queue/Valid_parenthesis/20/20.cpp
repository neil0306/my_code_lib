#include <iostream>
#include <stack>

using namespace std;

class Solution
{
    public:
        bool isValid(string s)
        {
            if(s.size() % 2 != 0){     // 个数不是偶数, 必不成对
                return false;
            }
            stack<char> temp;
            for(int i = 0; i < s.size(); i++)
            {
                if(s[i] == '(' ){
                    temp.push(')');
                }
                else if(s[i] == '{'){
                    temp.push('}');
                }
                else if(s[i] == '['){
                    temp.push(']');
                }
                else if(temp.empty() || temp.top() != s[i] ){      // case3: 栈为空了, 右括号太多 || case2: 左右括号不匹配
                    return false;
                }
                else{
                    temp.pop();     // 左右括号能匹配, 弹出栈顶元素, 继续下一次比较
                }
            }
            return temp.empty();    // case1: 遍历完, 栈内仍有元素, 说明左括号过多, 返回false; 要么就是刚好匹配, 栈为空, 返回true
        }
};

int main(void)
{
    string s;
    Solution ans;

    s = "()[]{}";
    bool res = ans.isValid(s);
    return 0;
}