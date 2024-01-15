#include <iostream>
#include <stack>
using namespace std;

class Solution {
public:
    string removeDuplicates(string s) {
        stack<char> temp;
        for(int i = 0; i < s.size(); ++i){
            if(temp.empty() || s[i] != temp.top()){
                temp.push(s[i]);
            }
            else{
                temp.pop();
            }
        }

        string res;
        // 先拼接字符串, 然后翻转
        while(!temp.empty()){
            res += temp.top();
            temp.pop();
        }
        reverse(res.begin(), res.end());

        // 不进行翻转的写法: 但是每次都会重新创建一个string, 执行效率很慢!
        // while(temp.empty() == false){
        //     res = temp.top() + res;     // 注意要让栈顶元素放在前面
        //     temp.pop();
        // }
        return res;
    }
};

int main(void)
{
    string s;
    Solution ans;

    s = "abbaca";
    cout << ans.removeDuplicates(s) << endl;
    return 0;
}