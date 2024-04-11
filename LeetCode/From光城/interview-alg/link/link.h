/*
 * @Author: 光城
 * @Date: 2020-09-30 08:16:00
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 08:16:31
 * @FilePath: /alg/link/link.h
 */
#pragma once
#include <iostream>
struct ListNode {
  int val;
  ListNode *next;
  ListNode(int x) : val(x), next(NULL) {}
};