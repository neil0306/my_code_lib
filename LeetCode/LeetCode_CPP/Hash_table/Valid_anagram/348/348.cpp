#include <iostream>

using namespace std;

class Solusion
{
    public:
        // --- 都是小写字母, 用数组解决 ----
        bool canConstruct(string ransomNote, string magazine) 
        {
            if(ransomNote.size() > magazine.size()) return false;
            int record[26] = {0};

            for(int i = 0; i < magazine.size(); ++i){
                record[magazine[i] - 'a']++;
            }

            for(int i = 0; i < ransomNote.size(); i++){
                record[ransomNote[i] - 'a']--;
            }

            for(int i = 0; i < 26; i++){
                if(record[i] < 0){
                    return false;
                }
            }
            return true;
        }
};

int main(void)
{
    string r, m;
    Solusion ans;

    r = "a", m = "b";
    r = "aa", m = "ab";
    r = "aa", m = "aba";
    bool res = ans.canConstruct(r, m);
    cout << (res == true ? "true" : "false") << endl; 

    return 0;
}