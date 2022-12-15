#include <stdarg.h>  //变长参数函数所需的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "c_container_auxiliary.h"
#include "cds.h"
#include "symbol_table.h"

Stack *stack_ast_pre = NULL;

Stack *stack_symbol_table = NULL;

List *ins_list = NULL;

SymbolTable *cur_symboltable = NULL;

ast *pre_astnode = NULL;

int yyparse(void);
int parser(char *input);

int main() {
  // 初始化指令链表
  ins_list = ListInit();
  ListSetClean(ins_list, CleanObject);

  // 初始化前置ast节点栈
  stack_ast_pre = StackInit();
  StackSetClean(stack_ast_pre, CleanObject);

  // 初始化符号表栈
  stack_symbol_table = StackInit();
  StackSetClean(stack_symbol_table, CleanObject);

  printf("开始遍历\n");
  char *input1 =
      "\
  int main() {\
    int a;\
    int b;\
    a = 5;\
    b = 10;\
    if (a == 5) {\
      if (b == 10) a = 25;\
    } else\
      a = a + 15;\
    return (a);\
  }\
  ";

  char *input2 = "int inc(){int a;float b;a= a + 20;}";
  parser(input2);

  void *element;
  int num = 10;
  ListFirst(ins_list, false);
  while (ListNext(ins_list, &element)) {
    // 打印出每条instruction的res的名字信息
    printf("%s\n", ((Instruction *)element)->res->name);
  }

  printf("遍历结束\n\n");
  ListDeinit(ins_list);

  return 0;
}
