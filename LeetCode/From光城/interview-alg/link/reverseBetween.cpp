/*
 * @Author: gcxzhang
 * @Date: 2020-07-05 12:25:23
 * @LastEditTime: 2020-09-30 11:36:52
 * @Description: 92. 反转链表 II
 * @FilePath: /alg/alg/link/reverseBetween.cpp
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
  ListNode* reverseBetween(ListNode* head, int m, int n) {
    if (!head || !head->next) return head;
    int start = 1;
    ListNode* dummyHead = new ListNode(-1);
    dummyHead->next = head;
    ListNode *pre = dummyHead, *cur = head;
    while (start < m) {
      pre = pre->next;
      start++;
    }
    cur = pre->next;

    ListNode* head_p = pre;

    while (m <= n) {
      ListNode* tail = cur->next;
      cur->next = pre;
      pre = cur;
      cur = tail;
      m++;
    }
    head_p->next->next = cur;
    head_p->next = pre;
    return dummyHead->next;
  }
};