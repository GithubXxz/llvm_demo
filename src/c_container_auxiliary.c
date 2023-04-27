#include "c_container_auxiliary.h"

Stack* stack_ast_pre = NULL;

Stack* stack_symbol_table = NULL;

Stack* stack_else_label = NULL;

Stack* stack_then_label = NULL;

// while条件为false时所要跳转的label栈
Stack* stack_while_then_label = NULL;

// while循环头(条件判断)
Stack* stack_while_head_label = NULL;

// 哈希表的初始化 用于查找调用的函数名和对应的函数label
HashMap* func_hashMap = NULL;

List* ins_list = NULL;

List* func_list = NULL;

// 用于生成cfg图通过label的名字查询对应的bblock
HashMap* bblock_hashmap = NULL;

HashSet* bblock_pass_hashset = NULL;

// 用于block的链状结构转换为CFG图结构
// dfs判断一个节点是否经过如果经过还需要取出节点建立边
// (char *name,HeadNode *node)
HashMap* bblock_to_dom_graph_hashmap = NULL;

// 出栈/链表删除 伴随运行的函数 我真他妈 心态崩了 学艺不精
void CleanObject(void* element) {}

unsigned HashKey(void* key) { return HashDjb2((char*)key); }

// 直接使用地址进行hash计算
unsigned HashKeyAddress(void* key) { return HashDjb2(key); }

// 设置一个hash_map的比较方式 示例为通过字符串排序
// 将键类型强转位字符串然后来比较
int CompareKey(void* lhs, void* rhs) {
  // printf("compare %s and %s\n", (char*)lhs, (char*)lhs);
  return strcmp((char*)lhs, (char*)lhs);
}

// 因为数据结构心态再一次崩掉
int CompareKeyAddress(void* lhs, void* rhs) {
  // 保证内容相同的同时地址相同 内容相同的先决条件
  return lhs != rhs;
}

void CleanHashMapKey(void* key) { free(key); }

void CleanHashMapKeyNotFree(void* key) {}

void CleanHashSetKey(void* key) {}

void CleanValue(void* value) {}

void CleanIntValue(void* value) { free((int*)value); }

void hashset_init(HashSet** self) {
  *self = HashSetInit();
  HashSetSetHash(*self, HashKeyAddress);
  HashSetSetCompare(*self, CompareKeyAddress);
  HashSetSetCleanKey(*self, CleanHashSetKey);
}

void hashset_init_string(HashSet** self) {
  *self = HashSetInit();
  HashSetSetHash(*self, HashKey);
  HashSetSetCompare(*self, CompareKey);
  HashSetSetCleanKey(*self, CleanHashMapKey);
}

void hashmap_init(HashMap** self) {
  *self = HashMapInit();
  HashMapSetHash(*self, HashKey);
  HashMapSetCompare(*self, CompareKey);
  HashMapSetCleanKey(*self, CleanHashMapKey);
  HashMapSetCleanValue(*self, CleanValue);
}

void hashmap_init_address(HashMap** self) {
  *self = HashMapInit();
  HashMapSetHash(*self, HashKeyAddress);
  HashMapSetCompare(*self, CompareKeyAddress);
  HashMapSetCleanKey(*self, CleanHashSetKey);
  HashMapSetCleanValue(*self, CleanValue);
}

void list_copy(List* dest, List* src) {
  void* element;
  ListFirst(src, false);
  while (ListNext(src, &element) != NULL) {
    ListPushBack(dest, element);
  }
}

void AllInit() {
  // 初始化指令链表
  ins_list = ListInit();
  ListSetClean(ins_list, CleanObject);

  // 初始化函数链表
  func_list = ListInit();
  ListSetClean(func_list, CleanObject);

  // 初始化前置ast节点栈
  stack_ast_pre = StackInit();
  StackSetClean(stack_ast_pre, CleanObject);

  // 初始化符号表栈
  stack_symbol_table = StackInit();
  StackSetClean(stack_symbol_table, CleanObject);

  // 初始化else的goto栈
  stack_else_label = StackInit();
  StackSetClean(stack_else_label, CleanObject);

  // while条件为false时所要跳转的label栈
  stack_then_label = StackInit();
  StackSetClean(stack_then_label, CleanObject);

  // while循环头(条件判断)
  stack_while_head_label = StackInit();
  StackSetClean(stack_while_head_label, CleanObject);

  // 初始化then的goto栈
  stack_while_then_label = StackInit();
  StackSetClean(stack_while_then_label, CleanObject);

  func_hashMap = HashMapInit();
  // 哈希表的初始化 用于查找调用的函数名和对应的函数label
  HashMapSetHash(func_hashMap, HashKey);
  HashMapSetCompare(func_hashMap, CompareKey);
  HashMapSetCleanKey(func_hashMap, CleanHashMapKey);
  HashMapSetCleanValue(func_hashMap, CleanValue);

  bblock_hashmap = HashMapInit();
  // 用于遍历生成cfg图结构时候查找bblock
  HashMapSetHash(bblock_hashmap, HashKey);
  HashMapSetCompare(bblock_hashmap, CompareKey);
  HashMapSetCleanKey(bblock_hashmap, CleanHashMapKey);
  HashMapSetCleanValue(bblock_hashmap, CleanValue);

  bblock_to_dom_graph_hashmap = HashMapInit();
  // 用于遍历生成cfg图结构时候查找bblock
  HashMapSetHash(bblock_to_dom_graph_hashmap, HashKey);
  HashMapSetCompare(bblock_to_dom_graph_hashmap, CompareKey);
  HashMapSetCleanKey(bblock_to_dom_graph_hashmap, CleanHashMapKey);
  HashMapSetCleanValue(bblock_to_dom_graph_hashmap, CleanValue);

  hashset_init(&bblock_pass_hashset);
}
