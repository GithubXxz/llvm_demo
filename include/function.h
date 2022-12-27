
#ifndef FUNCTION_H
#define FUNCTION_H
#include "cds.h"
#include "instruction.h"
#include "value.h"

struct _BasicBlock;
typedef struct _BasicBlock BasicBlock;

typedef struct _Function {
  // 指向label节点的value 等同于funcLabel节点里所包含的value*
  Value* label;
  BasicBlock* entry_bblock;
} Function;

#endif