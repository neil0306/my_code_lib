#include <iostream>
#include <stack>

using namespace std;

class Solution
{
    public:
        int evalPRN(vector<string> &tokens)
        {
            stack<long long> temp;          // LeetCode 测试数据改了, 要用long long才不会溢出
            for(string s : tokens){
                if(s == "+" || s == "-" || s == "*" || s == "/"){
                    long long b = temp.top();               // string如果是一个数字的话, 直接用long long可以隐式转换
                    temp.pop();

                    long long a = temp.top();
                    temp.pop();

                    if(s == "+") temp.push(a+b);
                    if(s == "-") temp.push(a-b);
                    if(s == "*") temp.push(a*b);
                    if(s == "/") temp.push(a/b);
                }
                else{
                    temp.push(stoll(s));                    // stoll: string to long long
                }
            }
            int res = temp.top();
            temp.pop();
            return res;
        }
};

int main(void)
{
    vector<string> s;
    Solution ans;

    s = {"2","1","+","3","*"};
    cout << ans.evalPRN(s) << endl;

    return 0;
}