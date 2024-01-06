#include <iostream>

using namespace std;

class Solution
{
    public:
        vector<int> spiralOrder(vector<vector<int> >& matrix) 
        {
            vector<int> res;                    // 存储遍历结果

            if(matrix.empty()){                 // 空矩阵, 直接返回
                return res;
            }

            int up = 0;                         // 矩阵能访问到的最上方index
            int down = matrix.size()-1;         // 矩阵能访问到的最下方index
            int left = 0;                       // 矩阵能访问到的最左侧index
            int right = matrix[0].size() -1;    // 矩阵能访问到的最右侧index

            // 遍历的不变量是: 左闭右闭
            while(true){
                // 遍历矩阵最上方一行
                for(int j = left; j <= right; ++j){
                    res.push_back(matrix[up][j]);   // 用up指定为矩阵最上面一行
                }
                if(++up > down){                    // 访问完一行, 更新上边界, 同时看看与下边界是否相交
                    break;
                }

                // 遍历矩阵最右侧一列
                for(int i = up; i <= down; ++i){
                    res.push_back(matrix[i][right]);  // 用right指定为矩阵最右侧一列
                }
                if(--right < left){                   // 访问完一列, 更新右边界, 同时看看与左边界是否相交
                    break;
                }

                // 遍历矩阵最下面一行
                for(int j = right; j >= left; --j){
                    res.push_back(matrix[down][j]);     // 用down指定为矩阵最下方一行
                }
                if(--down < up){                        // 访问完一行, 更新下边界, 同时看看与上边界是否相交
                    break;
                }

                // 遍历矩阵最左侧一列
                for(int i = down; i >= up; --i){
                    res.push_back(matrix[i][left]);
                }
                if(++left > right){                     // 访问完一列, 更新左边界, 同时看看与右边界是否相交
                    break;
                }
            }
            return res;
        }
};

int main(void)
{
    Solution res;

    vector<vector<int> > matrix = {{1,2,3},{4,5,6},{7,8,9}};
    // matrix = {{1,2,3,4},{5,6,7,8},{9,10,11,12}};
    // matrix = {{1}};
    // matrix = {{6,9,7}};

    auto ans = res.spiralOrder(matrix);

    for(size_t i = 0; i < ans.size(); i++){
        cout << ans[i] << " ";
    }
    cout << endl;
    
    return 0;
}

