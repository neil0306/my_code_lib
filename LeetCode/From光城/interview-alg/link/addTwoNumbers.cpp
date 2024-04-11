/*
 * @Author: gcxzhang
 * @Date: 2020-07-05 12:32:35
 * @LastEditTime: 2020-09-30 11:34:16
 * @Description: 面试题 02.05. 链表求和
 * @FilePath: /alg/alg/link/面试题 02.05. 链表求和.cpp
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
  ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode *p = l1, *q = l2;

    ListNode *dummyHead = new ListNode(-1), *dh = dummyHead;
    int carry = 0;
    while (p || q) {
      if (!p) {
        dh->next = new ListNode((carry + q->val) % 10);
        carry = (carry + q->val) / 10;
        q = q->next;
      } else if (!q) {
        dh->next = new ListNode((carry + p->val) % 10);
        carry = (carry + p->val) / 10;
        p = p->next;
      } else {
        dh->next = new ListNode((carry + p->val + q->val) % 10);
        carry = (carry + p->val + q->val) / 10;
        p = p->next;
        q = q->next;
      }
      dh = dh->next;
    }

    if (carry) dh->next = new ListNode(carry);
    return dummyHead->next;
  }
};