#include <iostream>

using namespace std;

class Solution
{
    public:
        vector<vector<string>> groupAnagrams(vector<string>& strs)
        {
            vector<vector<string> > res;
            unordered_map<string, int> hashMap;   // 用来判断排序后的单词是否存在, value值是int, 它用来区分不同的异位词字符串应该属于res的哪个部分
            string temp = "";
            int res_index = 0;                    // 用来区分不同的异位词, 同时也是res中存放不同 "字母异位词" 的index

            for(int i = 0; i < strs.size(); ++i){
                // 对每一个单词进行排序
                temp = strs[i];
                sort(temp.begin(), temp.end());

                if(hashMap.count(temp) != 0){   // 当前单词已经出现过
                    res[hashMap[temp]].push_back(strs[i]);  // 以 hashMap 的 value 作为索引, 将 strs[i] 放入res
                }
                else{   // 如果当前单词是第一次出现   
                    vector<string> vec(1, strs[i]);
                    res.push_back(vec); 
                    hashMap[temp] = res_index;
                    res_index++;                      // 用来区分不同的异位词, 每次使用完sub, sub都自增
                }
            }
            return res;
        }
};

int main(void)
{
    vector<string> strs;
    vector<vector<string>> res;

    strs = {"eat", "tea", "tan", "ate", "nat", "bat"};
    Solution ans;
    res = ans.groupAnagrams(strs);

    cout << "[ ";
    for(auto group:res){
        cout << "[ ";
        for(auto item:group){
            cout << item << " ";
        }
        cout << "] ";
    }
    cout << "]" << endl;

    return 0;
}