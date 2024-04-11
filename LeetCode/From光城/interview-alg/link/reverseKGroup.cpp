/*
 * @Author: gcxzhang
 * @Date: 2020-07-05 12:19:52
 * @LastEditTime: 2020-09-30 11:37:05
 * @Description: 25. K 个一组翻转链表
 * @FilePath: /alg/alg/link/reverseKGroup.cpp
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
  ListNode* reverseKGroup(ListNode* head, int k) {
    ListNode *dummyHead = new ListNode(-1), *p = dummyHead;
    dummyHead->next = head;
    ListNode* pre = NULL;

    while (true) {
      ListNode* tail = head;
      int count = k;
      while (count && tail) {
        tail = tail->next;
        count--;
      }

      if (count) {
        break;
      }
      ListNode* cur = head;
      while (cur != tail) {
        ListNode* r = cur->next;
        cur->next = pre;
        pre = cur;
        cur = r;
      }
      p->next = pre;
      head->next = tail;
      p = head;
      head = tail;
    }

    return dummyHead->next;
  }
};
