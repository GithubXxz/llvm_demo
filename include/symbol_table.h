#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "cds.h"

// 符号表 由指向父表的指针和一个哈希表组成
typedef struct _SymbolTable {
  struct _SymbolTable* father;
  //<char*,Value*> 名字和对应的value
  HashMap* symbol_map;
} SymbolTable;

void symbol_table_init(struct _SymbolTable*);

#endif