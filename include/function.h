#ifndef FUNCTION_H
#define FUNCTION_H
#include "cds.h"
#include "instruction.h"
#include "value.h"

struct _BasicBlock;

typedef struct _Function {
  // 指向label节点的value 等同于funcLabel节点里所包含的value*
  Value* label;
  // 入口基本块
  struct _BasicBlock* entry_bblock;
  // 出口基本块
  struct _BasicBlock* end_bblock;
  // 基本块的数量
  int num_of_block;
  // 每个变量的活跃区间
  List* all_var_live_interval;
  // 函数对应的cfg图
  struct _ALGraph* self_cfg;
  HashMap* var_localtion;
} Function;

void function_init(Function* this);

#endif