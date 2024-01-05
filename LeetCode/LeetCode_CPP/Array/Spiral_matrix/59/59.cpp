#include <iostream>

using namespace std;

class Solution
{
    public:
        vector<vector<int> > generateMatrix(int n)
        {
            vector<vector<int> > res(n, vector<int>(n,0));  // 使用构造函数构造 n*n 的二维数组, 元素全部初始化为0 
            int total_loop = n / 2;         // 矩阵在构造时的总旋转次数
            int mid_idx = n / 2;            // 矩阵中心位置的行,列坐标: n为3, idx=1, 中心点为[1,1]; n为5, idx=2, 中心点为[2,2]
            int count = 1;                  // 用于生成矩阵里的元素
            int offset = 1;                 // 每一条边的末尾点距离矩阵边缘的距离
            int start_x = 0, start_y = 0;   // 遍历的起始位置, x为行, y为列, (x,y)
            int i, j;                       // i控制行, j控制列, (i,j)

            while(total_loop --){
                i = start_x;
                j = start_y;
                
                // 上面一行
                for(; j < n - offset; j++){ // 不需要再提供初始值
                    res[i][j] = count;
                    count++;
                }

                // 右侧一列
                for(; i < n - offset; i++){
                    res[i][j] = count;
                    count++;
                }

                // 下面一行
                for(; j > start_y; j--){
                    res[i][j] = count;
                    count++;
                }

                // 左侧一列
                for(; i > start_x; i--){
                    res[i][j] = count;
                    count++;
                }
                
                // 更新下一圈位置: [x,y]沿着对角线方向移动, offset朝着矩阵中心缩小
                start_x++;
                start_y++;
                offset++;
            }

            // (n为奇数时)处理矩阵中心
            if(n%2 != 0){
                res[mid_idx][mid_idx] = count;
            }
            return res;
        }
};

int main(void)
{
    Solution s;
    auto res = s.generateMatrix(6);

    cout << "Show the result matrix: " << endl;
    for(size_t i = 0; i < res.size(); i++){
        for(size_t j = 0; j < res[i].size(); j++){
            cout << res[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
    return 0;
}