# 题目
题目来自"卡码网": 替换数字
- https://kamacoder.com/problempage.php?pid=1064

```txt
给定一个字符串 s，它包含小写字母和数字字符，请编写一个函数，将字符串中的字母字符保持不变，而将每个数字字符替换为number。

    例如，对于输入字符串 "a1b2c3"，函数应该将其转换为 "anumberbnumbercnumber"。
    对于输入字符串 "a5b"，函数应该将其转换为 "anumberb"

输入：
    一个字符串 s,s 仅包含小写字母和数字字符。
输出：
    打印一个新的字符串，其中每个数字字符都被替换为了number

样例输入：
    a1b2c3
样例输出：
    anumberbnumbercnumber

数据范围：
    1 <= s.length < 10000。
```

# 思路
思路很简单, 主要为难的是填充的过程, 由于数组是连续的内存空间, 所以如果`从前向后`填充, 则每填充一个数字, 就需要搬运后面所有的元素, 时间复杂度就会变成`O(n^2)`, 所以:
- 对于填充数组的问题, 我们一般`从后往前`填充, 这样时间复杂度就变成`O(n)`了.
- 对于C++而言, string是可以resize的, 因此不必重新申请一个新的数组, 并且此时使用的额外空间是相对固定的, 空间复杂度为O(1).

操作:
```txt
1. 统计数字和字母的个数, 计算出替换数组后的数组长度 = 数字的个数 * 6 + 字母的个数

2. 调整string的大小, C++使用 resize方法

3. 使用双指针i和j, i放在原字符串的长度末尾位置, 一个放在调整后的末尾, 从后往前填充, 当遇到数字时, s[j]不断往前走并倒着填充 "number" 这个单词, 遇到字母的时候, 将s[i] 填充给 s[j].
```

## 代码
```cpp
string replaceNumber(string &s)
{
    int cnt_num = 0;
    int ori_size = s.size();

    // 统计数字个数
    for(int i = 0; i < ori_size; i++){
        if('0' <= s[i] && s[i] <= '9'){
            cnt_num++;
        }
    }

    // 调整大小
    s.resize(ori_size + cnt_num * 5);   // 虽然 "number" 有6个字母, 但是我们替换的位置本身就有1个空位, 所以只需要扩大5倍
    int new_size = s.size();

    // 填充
    for(int i = ori_size-1, j =  new_size-1; j > i; i--, j--){
        if(s[i] < '0' || s[i] > '9'){   // 字符
            s[j] = s[i];
        }
        else{                           // 数字
            s[j] = 'r';
            s[j - 1] = 'e';
            s[j - 2] = 'b';
            s[j - 3] = 'm';
            s[j - 4] = 'u';
            s[j - 5] = 'n';
            j -= 5;
        }
    }
    return s;
}
```