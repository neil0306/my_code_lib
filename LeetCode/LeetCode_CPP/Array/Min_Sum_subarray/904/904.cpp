#include <iostream>

using namespace std;

class Solution
{
    public:
        int totalFruit(vector<int> & fruits)
        {
            int res = 0;
            int f_size = fruits.size();
            int valid = 0;                  // 用于记录窗口中种类的数量 

            int left = 0, right = 0;        // 窗口左右边界
            unordered_map<int, int> window; // 哈希表(python的字典), 用来记录不同元素的个数

            while(right < f_size){   // 遍历数组 & 右边界移动方式
                int fruit_a = fruits[right];

                // 如果进来的是新元素: 在哈希表中创建新 key-value; 元素种类+1
                if(window[fruit_a] == 0){
                    valid++;
                }
                // 不管是不是新种类, 总数都+1
                window[fruit_a]++;

                // 检查种类数量, 不满足窗口条件就从窗口左侧开始依次弹出元素 (窗口左边界移动方式)
                while(valid > 2){
                    int fruit_b = fruits[left];
                    window[fruit_b]--;

                    // 如果当前窗口已经没有水果种类, 则种类-1
                    if(window[fruit_b] == 0){
                        valid--;
                    }

                    // 移动左边界 
                    left++;
                }

                // 移动右边界, 遍历下一个元素
                right++;
            
                res = max(res, right - left);
                // debug
                // cout << "res: " << res << endl;
                // show_window(fruits, left, right);
            }
            return res;
        }

        // debug
        void show_window(vector<int> &fruits, int left, int right)
        {
            cout << "[" << left << ", " << right << "]: ";
            while(left < right){
                cout << fruits[left++] << " ";
            }
            cout << endl;
        }
};

int main(void)
{
    vector<int> fruits = {0,1,2,2};
    Solution res;

    cout << res.totalFruit(fruits) << endl;
    return 0;
}