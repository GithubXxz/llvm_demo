#include "symbol_table.h"

#include "c_container_auxiliary.h"

extern Stack* stack_symbol_table;

void symbol_table_init(struct _SymbolTable* self) {
  self->symbol_map = HashMapInit();
  // 哈希表的初始化
  HashMapSetHash(self->symbol_map, HashKey);
  HashMapSetCompare(self->symbol_map, CompareKey);
  HashMapSetCleanKey(self->symbol_map, CleanKey);
  HashMapSetCleanValue(self->symbol_map, CleanValue);

  // 如果栈不为空
  if (StackSize(stack_symbol_table) != 0) {
    void* element;
    // 取出栈顶符号表元素
    StackTop(stack_symbol_table, &element);
    // 将当前符号表的父节点指向栈顶区域块
    self->father = (SymbolTable*)element;
    // 将当前符号表压栈
    StackPush(stack_symbol_table, self);
  } else {
    // 将该区域块的父节点指向null
    self->father = NULL;
    // 将该区域块压栈
    StackPush(stack_symbol_table, self);
  };
};