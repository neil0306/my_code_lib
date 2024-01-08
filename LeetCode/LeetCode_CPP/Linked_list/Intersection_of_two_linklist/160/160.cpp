#include <iostream>

using namespace std;

struct ListNode
{
    int val;
    ListNode * next;
    ListNode() : val(0), next(nullptr){}
    ListNode(int v) : val(v), next(nullptr) {}
    ListNode(int v, ListNode * n): val(v), next(n){}
};

class Solution
{
    public:
        ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) 
        {
            ListNode * CurA = headA;
            ListNode * CurB = headB;
            int m = 0, n = 0;

            // 统计两个链表的长度
            while(CurA != nullptr){
                m++;
                CurA = CurA->next;
            } 
            while(CurB != nullptr){
                n++;
                CurB = CurB->next;
            }
            
            // 移动指针, 让两个链表从 Cur 指针之后的节点长度一致
            CurA = headA;
            CurB = headB;
            if(m > n){
                for(int i = 0; i < m-n; i++){
                    CurA = CurA->next;
                }
            }
            else if(m < n){
                for(int i = 0; i < n - m; i++){
                    CurB = CurB->next;
                }
            }

            // 逐一判断指针地址是否相等
            while(CurA != nullptr){
                if(CurA == CurB){
                    return CurA;
                }
                CurA = CurA->next;
                CurB = CurB->next;
            }

            return nullptr;
        }
};

int main(void)
{
    /* 这里不写测试了... 生成交叉链表有点烦人... */
    return 0;
}