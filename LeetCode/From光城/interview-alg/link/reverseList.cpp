/*
 * @Author: gcxzhang
 * @Date: 2020-07-05 12:22:51
 * @LastEditTime: 2020-09-30 11:34:33
 * @Description: 剑指 Offer 24. 反转链表
 * @FilePath: /alg/alg/link/reverseList.cpp
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
  ListNode* reverseList(ListNode* head) {
    if (!head || !head->next) return head;
    ListNode* p = reverseList(head->next);
    head->next->next = head;
    head->next = NULL;
    return p;
  }
};
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
 public:
  ListNode* reverseList(ListNode* head) {
    if (!head || !head->next) return head;
    ListNode *pre = NULL, *cur = head, *tail = NULL;
    while (cur) {
      tail = cur->next;
      cur->next = pre;
      pre = cur;
      cur = tail;
    }
    return pre;
  }
};