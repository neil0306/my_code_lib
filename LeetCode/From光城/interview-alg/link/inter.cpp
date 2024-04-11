/*
 * @Author: gcxzhang
 * @Date: 2020-07-03 21:36:08
 * @LastEditTime: 2020-09-30 11:36:07
 * @Description: 160. 相交链表
 * https://leetcode-cn.com/problems/intersection-of-two-linked-lists/
 * @FilePath: /alg/alg/link/inter.cpp
 */
#include <iostream>

#include "link.h"
using namespace std;

// 160. 相交链表
// https://leetcode-cn.com/problems/intersection-of-two-linked-lists/
class Solution {
 public:
  ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
    // a+b+c = c+b+a

    ListNode *p = headA, *q = headB;
    while (p != q) {
      p = p == NULL ? headB : p->next;
      q = q == NULL ? headA : q->next;
    }
    return p;
  }
};

// 2.两数相加
// https://leetcode-cn.com/problems/add-two-numbers/
class Solution {
 public:
  ListNode *addTwoNumbers(ListNode *l1, ListNode *l2) {
    int carry = 0;
    ListNode *p = l1, *q = l2;
    ListNode *dummyHead = new ListNode(-1), *dm = dummyHead;
    while (p || q) {
      if (p == NULL && q) {
        while (q) {
          dm->next = new ListNode((q->val + carry) % 10);
          carry = (q->val + carry) / 10;
          q = q->next;
          dm = dm->next;
        }
        break;
      }
      if (p && q == NULL) {
        while (p) {
          dm->next = new ListNode((p->val + carry) % 10);
          carry = (p->val + carry) / 10;
          p = p->next;
          dm = dm->next;
        }
        break;
      }

      dm->next = new ListNode((p->val + q->val + carry) % 10);
      carry = (p->val + q->val + carry) / 10;
      p = p->next;
      q = q->next;
      dm = dm->next;
    }
    if (carry) dm->next = new ListNode(carry);
    return dummyHead->next;
  }
};