/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:16
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 11:37:17
 * @Description: 24. 两两交换链表中的节点
 * @FilePath: /alg/alg/link/swapPairs.cpp
 */

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
#include "link.h"
class Solution {
 public:
  ListNode* swapPairs(ListNode* head) {
    if (!head || !head->next) return head;
    ListNode* q = head->next;
    head->next = swapPairs(q->next);
    q->next = head;
    return q;
  }
};

class Solution {
 public:
  ListNode* swapPairs(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode* dummyHead = new ListNode(-1);
    dummyHead->next = head;
    ListNode *p = dummyHead, *q;
    while (p->next && p->next->next) {
      q = p->next;
      // -1 1 2 3
      // -1->2
      p->next = q->next;
      // 1->3
      q->next = q->next->next;
      // 2->1
      p->next->next = q;
      p = q;
    }
    return dummyHead->next;
  }
};