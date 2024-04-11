/*
 * @Author: 光城
 * @Date: 2020-09-30 07:54:16
 * @LastEditors: 光城
 * @LastEditTime: 2020-09-30 10:27:13
 * @Description: 手撕字符串系列
 * @FilePath: /alg/string/helper.cpp
 */
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

namespace lightcity {
void memcpy(void* dest, const void* src, size_t n) {
  if (!dest || !src) {
    return;
  }
  char* d = (char*)dest;
  char* s = (char*)src;
  if (s < d && s + n > d) {
    d = d + n - 1;
    s = s + n - 1;
    while (n--) {
      *d-- = *s--;
    }
  } else {
    while (n--) {
      *d++ = *s++;
    }
  }
}
char* strcpy(char* dest, const char* src) {
  if (!dest || !src) return NULL;
  char* d = dest;
  while ((*d++ = *src++) != '\0') {
  };
  return dest;
}
char* strncpy(char* dest, const char* src, size_t n) {
  if (!dest || !src) return NULL;
  size_t i = 0;
  for (; i < n && src[i] != '\0'; i++) dest[i] = src[i];

  for (; i < n; i++) {
    dest[i] = '\0';
  }
  return dest;
}

size_t strlen(const char* s) {
  if (!s) return 0;
  size_t len = 0;
  while ((*s++) != '\0') {
    len++;
  }
  return len;
}

char* strstr(const char* haystack, const char* needle) {
  if (!haystack || !needle) return 0;
  size_t hlen = strlen(haystack);
  size_t nlen = strlen(needle);
  char* hs = (char*)haystack;
  for (size_t i = 0; i < hlen - nlen; i++) {
    size_t j = 0;
    for (j = 0; j < nlen; j++) {
      if (haystack[i + j] != needle[j]) break;
    }
    if (j == nlen) return hs + i;
  }
  return NULL;
}
char* strcat(char* dest, const char* src) {
  if (!dest || !src) return NULL;
  char* d = dest;

  while ((*d++) != '\0') {
  }
  while ((*src) != '\0') {
    *d++ = *src++;
  }
  *d = '\0';
  return dest;
}
int strcmp(const char* s1, const char* s2) {
  while (*s1 == *s2) {
    if (*s1 == '\0') break;
    s1++;
    s2++;
  }
  return *s1 - *s2;
}
};  // namespace lightcity
int main() {
  char dest[10];
  const char* src = "1203lop";
  cout << lightcity::strlen(src) << endl;
  cout << strlen(src) << endl;
  lightcity::strcpy(dest, src);
  cout << dest << endl;

  src = "1223";
  lightcity::memcpy(dest, src, strlen(src));
  cout << dest << endl;  // 可以看到遇到str\0不结束

  cout << lightcity::strlen(src) << endl;

  const char* s1 = "abcdegdefghi";
  const char* s2 = "def";
  cout << "============" << endl;
  cout << lightcity::strstr(s1, s2) << endl;
  cout << strstr(s1, s2) << endl;
  char s3[100];
  cout << strcat(s3, s1) << endl;
  cout << lightcity::strcat(s3, s1) << endl;
  char* ss1 = "a";
  char* ss2 = "b";
  cout << strcmp(ss1, ss2) << endl;
  cout << lightcity::strcmp(ss1, ss2) << endl;
  return 0;
}