#include <iostream>
using namespace std;

struct ListNode
{
    int val;
    ListNode * next;
    ListNode() : val(0), next(nullptr){}
    ListNode(int v) : val(v), next(nullptr){}
    ListNode(int v, ListNode * n) : val(v), next(n) {}
};

class Solution
{
    public:
        ListNode* removeNthFromEnd(ListNode* head, int n) 
        {
            if(head == nullptr){
                return head;
            }

            // 创建虚拟头结点
            ListNode * dummyhead = new ListNode(0);
            dummyhead->next = head;

            ListNode * fast = dummyhead, * slow = dummyhead;
            // fast 先走n+1步 (题目的提示中已经说明 n 不会大于链表节点数量)
            while(n-- >= 0){
                fast = fast->next;
            }
            
            // 同时移动两个指针, 直到 fast 是链表最后一个节点, 此时slow就是倒数第n个节点前趋点
            while(fast != nullptr){
                fast = fast->next;
                slow = slow->next;
            }

            // 删除 slow->next 指向的节点
            ListNode * temp = slow->next;
            slow->next = temp->next;
            delete temp;

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
    vector<int> nums = {1,2,3,4,5,6};
    int n = 1;
    Solution ans;

    ListNode * head = ans.createLinkedList(nums, false);
    ans.showLinkedList(head);

    cout << " ----- After Deleting --------" << endl;
    head = ans.removeNthFromEnd(head, n);
    ans.showLinkedList(head);
    return 0;
}