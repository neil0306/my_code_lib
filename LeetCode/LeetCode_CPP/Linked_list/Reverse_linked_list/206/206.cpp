#include <iostream>
using namespace std;

struct ListNode
{
    int val;
    ListNode * next;
    ListNode() : val(0), next(nullptr){}
    ListNode(int v) : val(v), next(nullptr){}
    ListNode(int v, ListNode* n) : val(v), next(n){}
};

class Solution
{
    public:
        ListNode* reverseList(ListNode* head) 
        {
            // 创建虚拟头结点
            ListNode * dummyhead = new ListNode(0);
            dummyhead->next = nullptr;

            ListNode * cur = head;      // 当前处理的节点
            while(cur != nullptr){
                // 暂存链表后续节点, 防止原链表指针丢失
                ListNode * temp = cur->next; 
        
                // 头插法: 每次新节点都往链表头放入
                cur->next = dummyhead->next;
                dummyhead->next = cur;          // 此时cur指针位于 dummyhead 的链表上

                // 把cur指针重置回未处理的链表表头
                cur = temp;
            }

            return dummyhead->next;
        }


        // ---------------- debug ------------------- 
        // 用数组创建链表
        ListNode * createLinkedList(vector<int> arr, bool HeadInsert)
        {
            if(HeadInsert){
                // 头插法
                struct ListNode * head = nullptr;
                for(size_t i = 0; i < arr.size(); i++){
                    ListNode * newNode = new ListNode(arr[i]);
                    newNode->next = head;
                    head = newNode;
                }
                return head;
            }
            else{
                // 尾插法
                struct ListNode * dummyhead = new ListNode(0);
                struct ListNode * cur = dummyhead;
                for(size_t i = 0; i < arr.size(); i++){
                    struct ListNode * newPt = new ListNode(arr[i]);
                    cur->next = newPt;
                    cur = cur->next; 
                }
                return dummyhead->next;
            }

        }

        // 打印链表
        void showLinkedList(ListNode * head)
        {
            struct ListNode * pt = head;

            cout << "Your Linked-List: ";
            while(pt != nullptr){
                cout << pt->val << " ";
                pt = pt->next;
            }
            cout << endl;
        }
};

int main(void)
{
    vector<int> nums = {1,2,3,4,5};
    // nums = {};
    Solution ans;

    ListNode * head = ans.createLinkedList(nums, false);
    ans.showLinkedList(head);

    head = ans.reverseList(head);
    ans.showLinkedList(head);
    return 0;
}
