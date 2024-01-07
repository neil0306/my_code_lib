#include <iostream>

using namespace std;

/* 单链表 */
class MyLinkedList {
    private:
        struct ListNode
        {
            int val;
            ListNode * next;
            ListNode():val(0), next(nullptr){}
            ListNode(int value) : val(value), next(nullptr){}
            ListNode(int value, ListNode * n) : val(value), next(n){}
        };
        int length;
        ListNode * dummyhead;           // 统一使用虚拟头结点

    public:
        MyLinkedList()  // 构造函数
        {
            dummyhead = new ListNode();
            length = 0;
        }
        
        /*
            获取链表中下标为 index 的节点的值。
                如果下标无效，则返回 -1 。
        */
        int get(int index)
        {
            if(index >= 0 && index < length){
                ListNode * cur = dummyhead->next;       // 注意这里使用了虚拟头结点
                while(index-- > 0){
                    cur = cur->next;
                }
                return cur->val;
            }
            else{
                return -1;
            }
        }
        

        /*
            将一个值为 val 的节点插入到链表中第一个元素之前。
                在插入完成后，新节点会成为链表的第一个节点。
        */
        void addAtHead(int val) 
        {
            if(val < 0 || val > 1000) return;       // 题目的提示里给了val的范围...md

            ListNode * cur = new ListNode(val);
            cur->next = dummyhead->next;            // 注意使用了虚拟头结点
            dummyhead->next = cur;
            length++;
        }

        /* 
            将一个值为 val 的节点追加到链表中作为链表的最后一个元素。
        */
        void addAtTail(int val) 
        {
            if(val < 0 || val > 1000) return;       // 题目的提示里给了val的范围...md

            // 找到末尾
            ListNode * cur = dummyhead;
            while(cur->next != nullptr){
                cur = cur->next;
            }

            // 新增节点
            ListNode * newNode = new ListNode(val);
            cur->next = newNode;
            length++;
        }
        
        /*
            将一个值为 val 的节点插入到链表中下标为 index 的节点之前。
                如果 index 等于链表的长度，那么该节点会被追加到链表的末尾。
                如果 index 比长度更大，该节点将 不会插入 到链表中。
        */
        void addAtIndex(int index, int val) 
        {
            if(val < 0 || val > 1000) return;       // 题目的提示里给了val的范围...md

            if(index <= 0){                         // index 小于0时, 也插入到链表头
                addAtHead(val);
            }
            else if(index == length){               // index 从0开始计算
                addAtTail(val);
            }
            else if(index < length){
                // 找到前趋点 (找出 index -1 位置)
                ListNode * cur = dummyhead;
                while(index-- > 0){
                    cur = cur->next;
                }
                
                ListNode * newNode = new ListNode(val);
                newNode->next = cur->next;
                cur->next = newNode;
                length++;
            }
            else{       //  index > length
                return;
            }
        }
        
        /*
            如果下标有效，则删除链表中下标为 index 的节点。
        */
        void deleteAtIndex(int index) 
        {
            if(index >= 0 && index < length){
                // 找前趋点 (走到 index -1 位置)
                ListNode * cur = dummyhead;
                while(index-- > 0){
                    cur = cur->next;
                }

                // 删节点
                ListNode * temp = cur->next;        // 暂存被删节点
                cur->next = temp->next;
                delete temp;
                length--;
            }
        }

        void Show_LinkedList()
        {
            ListNode * cur = dummyhead->next;
            cout << "Linked List: ";
            while(cur != nullptr){
                cout << cur->val << " ";
                cur = cur->next;
            }
            cout << endl;
        }
};

int main(void)
{
    MyLinkedList * myLinkedList = new MyLinkedList();
    myLinkedList->addAtHead(1);
    myLinkedList->addAtTail(3);
    myLinkedList->addAtIndex(1, 2);    // 链表变为 1->2->3
    myLinkedList->Show_LinkedList();
    cout << myLinkedList->get(1) << endl;         // 返回 2
    myLinkedList->deleteAtIndex(1);    // 现在，链表变为 1->3
    myLinkedList->Show_LinkedList();
    cout << myLinkedList->get(1) << endl;              // 返回 3
    return 0;
}