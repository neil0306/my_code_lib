#include <iostream>
using namespace std;

class Solution
{
    public:
        void reverseString(vector<char> &s)
        {
            int left = 0, right = s.size()-1;
            while(left < right){
                char temp = s[left];
                s[left++] = s[right];
                s[right--] = temp;
            }
        }
};

int main(void)
{
    vector<char> s;
    Solution ans;

    s = {'h','e','l','l','o'};
    cout << "original string: ";
    for(char c : s){
        cout << c;
    }
    cout << endl;

    ans.reverseString(s);

    cout << "After reverse: ";
    for(char c : s){
        cout << c;
    }
    cout << endl;
    return 0;
}