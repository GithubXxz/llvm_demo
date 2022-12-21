#include "c_container_auxiliary.h"

Stack* stack_ast_pre = NULL;

Stack* stack_symbol_table = NULL;

Stack* stack_else_label = NULL;

Stack* stack_then_label = NULL;

HashMap* func_hashMap = NULL;

List* ins_list = NULL;

// 出栈/链表删除 伴随运行的函数 我真他妈 心态崩了 学艺不精
void CleanObject(void* element) {}

unsigned HashKey(void* key) { return HashDjb2((char*)key); }

// 设置一个hash_map的比较方式 示例为通过字符串排序
// 将键类型强转位字符串然后来比较
int CompareKey(void* lhs, void* rhs) {
  // printf("compare %s and %s\n", (char*)lhs, (char*)lhs);
  return strcmp((char*)lhs, (char*)lhs);
}

void CleanKey(void* key) { free(key); }

void CleanValue(void* value) {}

void AllInit() {
  // 初始化指令链表
  ins_list = ListInit();
  ListSetClean(ins_list, CleanObject);

  // 初始化前置ast节点栈
  stack_ast_pre = StackInit();
  StackSetClean(stack_ast_pre, CleanObject);

  // 初始化符号表栈
  stack_symbol_table = StackInit();
  StackSetClean(stack_symbol_table, CleanObject);

  // 初始化else的goto栈
  stack_else_label = StackInit();
  StackSetClean(stack_else_label, CleanObject);

  // 初始化then的goto栈
  stack_then_label = StackInit();
  StackSetClean(stack_then_label, CleanObject);

  func_hashMap = HashMapInit();
  // 哈希表的初始化
  HashMapSetHash(func_hashMap, HashKey);
  HashMapSetCompare(func_hashMap, CompareKey);
  HashMapSetCleanKey(func_hashMap, CleanKey);
  HashMapSetCleanValue(func_hashMap, CleanValue);
}