#include "c_container_auxiliary.h"

// 出栈伴随运行的函数 我真他妈 心态崩了 学艺不精
void CleanObject(void* element) {}

unsigned HashKey(void* key) { return HashDjb2((char*)key); }

// 设置一个hash_map的比较方式 示例为通过字符串排序
// 将键类型强转位字符串然后来比较
int CompareKey(void* lhs, void* rhs) {
  // printf("compare %s and %s\n", (char*)lhs, (char*)lhs);
  return strcmp((char*)lhs, (char*)lhs);
}

void CleanKey(void* key) { free(key); }

void CleanValue(void* value) { free(value); }