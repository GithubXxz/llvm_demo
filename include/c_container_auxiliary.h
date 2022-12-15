#ifndef C_CONTAINER_AUXILIARY
#define C_CONTAINER_AUXILIARY

#include "cds.h"

void CleanObject(void* element);

unsigned HashKey(void* key);

// 设置一个hash_map的比较方式 示例为通过字符串排序
// 将键类型强转位字符串然后来比较
int CompareKey(void* lhs, void* rhs);

void CleanKey(void* key);

void CleanValue(void* value);

#endif
