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
        ListNode* removeElements(ListNode* head, int val)
        {
            struct ListNode * dummy_head = new ListNode();      // 冗余头结点 dummy head, 它的next指向链表表头
            dummy_head->next = head;
            struct ListNode * temp = nullptr;                   // 临时存放待删节点
            struct ListNode * pre = dummy_head;                 // 遍历链表使用的节点

            // 遍历整个链表
            while(pre->next != nullptr){    // 由于使用了dummy head, 此时它也能处理空链表的情况
                // 找到待删节点的前趋点
                while(pre->next != nullptr && pre->next->val != val){
                    pre = pre->next;
                }

                // 判断pre是否为尾结点
                if(pre->next == nullptr){
                    break;
                }

                // 删除操作
                temp = pre->next;           // 保存待删节点的指针
                pre->next = temp->next;     // 绕过待删节点
                delete temp;
            }

            return dummy_head->next;
        }

        // 创建链表
        ListNode * createLinkedList(vector<int> arr)
        {
            struct ListNode * head = nullptr;
            for(size_t i = 0; i < arr.size(); i++){
                ListNode * newNode = new ListNode(arr[i]);
                newNode->next = head;
                head = newNode;
            }
            return head;
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
    Solution res;
    int val = 6;

    vector<int> nums = {1,2,6,3,4,5,6};
    // nums = {};
    // nums = {7,7,2,7,7}, val = 7;

    // 创建链表
    ListNode * head = res.createLinkedList(nums);
    res.showLinkedList(head);

    // 删除所有值为 val 的元素
    head = res.removeElements(head, val);
    res.showLinkedList(head);

    return 0;
}