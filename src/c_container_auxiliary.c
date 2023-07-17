#include "c_container_auxiliary.h"
#include "instruction.h"

Stack *stack_ast_pre = NULL;

Stack *stack_symbol_table = NULL;

Stack *stack_else_label = NULL;

Stack *stack_then_label = NULL;

// while条件为false时所要跳转的label栈
Stack *stack_while_then_label = NULL;

// while循环头(条件判断)
Stack *stack_while_head_label = NULL;

Stack *stack_param = NULL;

// 哈希表的初始化 用于查找调用的函数名和对应的函数label
HashMap *func_hashMap = NULL;

List *ins_list = NULL;

List *func_list = NULL;

List *global_var_list = NULL;

// 用于生成cfg图通过label的名字查询对应的bblock
HashMap *bblock_hashmap = NULL;

HashSet *bblock_pass_hashset = NULL;

// 用于block的链状结构转换为CFG图结构
// dfs判断一个节点是否经过如果经过还需要取出节点建立边
// (char *name,HeadNode *node)
HashMap *bblock_to_dom_graph_hashmap = NULL;

HashMap *global_array_init_hashmap = NULL;

// 出栈/链表删除 伴随运行的函数 我真他妈 心态崩了 学艺不精
void CleanObject(void *element) {}

unsigned HashKey(void *key) { return HashDjb2((char *)key); }

// 直接使用地址进行hash计算
unsigned HashKeyAddress(void *key) { return HashDjb2(key); }

// 设置一个hash_map的比较方式 示例为通过字符串排序
// 将键类型强转位字符串然后来比较
int CompareKey(void *lhs, void *rhs) {
  // printf("compare %s and %s\n", (char*)lhs, (char*)lhs);
  return strcmp((char *)lhs, (char *)rhs);
}

// 因为数据结构心态再一次崩掉
int CompareKeyAddress(void *lhs, void *rhs) {
  // 保证内容相同的同时地址相同 内容相同的先决条件
  return lhs != rhs;
}

void CleanHashMapKey(void *key) { free(key); }

void CleanHashMapKeyNotFree(void *key) {}

void CleanHashSetKey(void *key) {}

void CleanValue(void *value) {}

void CleanIntValue(void *value) { free((int *)value); }

void hashset_init(HashSet **self) {
  *self = HashSetInit();
  HashSetSetHash(*self, HashKeyAddress);
  HashSetSetCompare(*self, CompareKeyAddress);
  HashSetSetCleanKey(*self, CleanHashSetKey);
}

void hashset_init_string(HashSet **self) {
  *self = HashSetInit();
  HashSetSetHash(*self, HashKey);
  HashSetSetCompare(*self, CompareKey);
  HashSetSetCleanKey(*self, CleanHashMapKey);
}

void hashmap_init(HashMap **self) {
  *self = HashMapInit();
  HashMapSetHash(*self, HashKey);
  HashMapSetCompare(*self, CompareKey);
  HashMapSetCleanKey(*self, CleanHashMapKey);
  HashMapSetCleanValue(*self, CleanValue);
}

void hashmap_init_address(HashMap **self) {
  *self = HashMapInit();
  HashMapSetHash(*self, HashKeyAddress);
  HashMapSetCompare(*self, CompareKeyAddress);
  HashMapSetCleanKey(*self, CleanHashSetKey);
  HashMapSetCleanValue(*self, CleanValue);
}

void list_copy(List *dest, List *src) {
  void *element;
  ListFirst(src, false);
  while (ListNext(src, &element)) {
    ListPushBack(dest, element);
  }
}

typedef struct sys_fun_info {
  char *func_name;
  TypeID return_type;
  int param_num;
} sys_func_info;

void system_func_init() {
  sys_func_info sysy[20] = {{"getint", IntegerTyID, 0},
                            {"getch", IntegerTyID, 0},
                            {"getarray", IntegerTyID, 1},
                            {"getfloat", FloatTyID, 0},
                            {"getfarray", IntegerTyID, 1},
                            {"putint", VoidTyID, 1},
                            {"putch", VoidTyID, 1},
                            {"putarray", VoidTyID, 2},
                            {"putfloat", VoidTyID, 1},
                            {"putfarray", VoidTyID, 2},
                            {NULL, VoidTyID, 0}};
  for (int i = 0;; i++) {
    if (sysy[i].func_name == NULL)
      return;
    Value *func_label_ins = (Value *)ins_new_no_operator_v2(FuncLabelOP);
    // 添加变量的名字
    func_label_ins->name = strdup(sysy[i].func_name);
    func_label_ins->VTy->TID = FuncLabelTyID;
    func_label_ins->pdata->symtab_func_pdata.return_type = sysy[i].return_type;
    func_label_ins->pdata->symtab_func_pdata.param_num = sysy[i].param_num;

    // 将函数的<name,label>插入函数表
    char *sys_func_name = strdup(sysy[i].func_name);
    HashMapPut(func_hashMap, sys_func_name, func_label_ins);
  }
}

void AllInit() {
  // 初始化指令链表
  ins_list = ListInit();
  ListSetClean(ins_list, CleanObject);

  // 初始化全局变量链条
  global_var_list = ListInit();
  ListSetClean(global_var_list, CleanObject);

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

  stack_param = StackInit();
  StackSetClean(stack_param, CleanObject);

  hashmap_init(&func_hashMap);
  system_func_init();

  hashmap_init(&bblock_hashmap);

  hashmap_init(&bblock_to_dom_graph_hashmap);

  hashmap_init(&global_array_init_hashmap);

  hashset_init(&bblock_pass_hashset);
}
