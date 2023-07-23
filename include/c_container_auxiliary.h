#ifndef C_CONTAINER_AUXILIARY
#define C_CONTAINER_AUXILIARY

#include "cds.h"
#include "container/list.h"
#include <sys/time.h>

#define PRINT_OK
// #define TIMER_TEST

static struct timeval begin, end;

#ifdef TIMER_TEST
#define TIMER_BEGIN gettimeofday(&begin, 0)
#else
#define TIMER_BEGIN
#endif

#ifdef TIMER_TEST
#define TIMER_END(log)                                                         \
  gettimeofday(&end, 0);                                                       \
  {                                                                            \
    long seconds = end.tv_sec - begin.tv_sec;                                  \
    long microseconds = end.tv_usec - begin.tv_usec;                           \
    double elapsed = seconds + microseconds * 1e-6;                            \
    printf("%s time: %lf\n", log, elapsed);                                    \
  }
#else
#define TIMER_END(log)
#endif

#define SEQ(x, y) ((x) && (y) && (!strcmp(x, y)))

void CleanObject(void *element);

unsigned HashKey(void *key);

unsigned HashKeyAddress(void *key);

// 设置一个hash_map的比较方式 示例为通过字符串排序
// 将键类型强转位字符串然后来比较
int CompareKey(void *lhs, void *rhs);

int CompareKeyAddress(void *lhs, void *rhs);

void CleanKeyNotFree(void *key);

void CleanHashMapKey(void *key);

void CleanHashSetKey(void *key);

void CleanValue(void *value);

void CleanIntValue(void *value);

void AllInit();

void clear_list_nullptr(List *self);

void hashset_init(HashSet **self);

void hashset_init_string(HashSet **self);

void hashmap_init(HashMap **self);

void list_copy(List *dest, List *src);

void treemap_init(TreeMap **self);

#endif
