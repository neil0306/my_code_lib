#include <iostream>

using namespace std;

struct ListNode
{
    int val;
    ListNode * next;
    ListNode():val(0), next(nullptr){}
    ListNode(int v):val(v), next(nullptr){}
    ListNode(int v, ListNode * n):val(v), next(n){}
};

class Solution
{
    public:
        ListNode *detectCycle(ListNode *head) 
        {
            // 如果是空链表, 直接返回, 不做处理
            if(head == nullptr){
                return nullptr;
            }

            ListNode * fast = head, *slow = head;         // 快慢指针
            ListNode * index1 = head, * index2 = nullptr; // 用来寻找环的入口

            // 判断是否有环
            while(fast->next != nullptr && fast->next->next != nullptr){
                fast = fast->next->next;                // fast 每次走2步
                slow = slow->next;                      // slow 每次走1步

                if(fast == slow){
                    break;
                }
            }
            if(fast->next == nullptr || fast->next->next == nullptr){
                return nullptr;     // 无环
            }

            // 已确定有环, 从相遇位置开始找环入口
            index2 = fast;
            while(index1 != index2){
                index1 = index1->next;
                index2 = index2->next;
            }
            return index1;
        }
};


int main(void)
{
    // 不写测试了, 创建环形链表麻烦...
    return 0;
}