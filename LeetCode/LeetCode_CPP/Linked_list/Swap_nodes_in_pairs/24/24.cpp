#include <iostream>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution
{
    public:
        ListNode* swapPairs(ListNode* head) 
        {
            ListNode * dummyhead = new ListNode(0);
            dummyhead->next = head;

            ListNode * cur = dummyhead;

            /* 待交换的两个节点分别是 cur->next (这里称为节点1) 和 cur->next->next (这里称为节点2) */
            while(cur->next != nullptr && cur->next->next != nullptr){    // 首先确保准备交换的两个节点都不是空节点, 如果节点2为空节点, 则直接结束
                ListNode *temp1 = cur->next;                // 暂存指向节点1的指针
                ListNode *temp2 = cur->next->next->next;    // 暂存指向节点2后面一个节点的指针

                cur->next = cur->next->next;    // 步骤1: 完成后, cur->next 指向节点2
                cur->next->next = temp1;        // 步骤2: 完成后, 节点2->next 指向节点1
                cur->next->next->next = temp2;  // 步骤3: 节点1->next 指向节点2后一个节点

                cur = cur->next->next;          // 更新位置: 移动到(原来)节点2的位置
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
    Solution ans;

    ListNode *head =  ans.createLinkedList(nums, false);
    ans.showLinkedList(head);

    head = ans.swapPairs(head);
    ans.showLinkedList(head);

    return 0;
}