#include <stdarg.h>  //变长参数函数所需的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "c_container_auxiliary.h"
#include "cds.h"
#include "symbol_table.h"

extern List *ins_list;

SymbolTable *cur_symboltable = NULL;

Value *return_val = NULL;

ast *pre_astnode = NULL;

int yyparse(void);
int parser(char *input);

char *op_string[] = {"AddOP",
                     "SubOP",
                     "MulOP",
                     "DivOP",
                     "GotoOP",
                     "GotoWithConditionOP",
                     "CallOP",
                     "FunBeginOP",
                     "FunEndOP",
                     "ReturnOP",
                     "ParamOP",
                     "AssignOP",
                     "CallWithReturnValueOP",
                     "NotEqualOP",
                     "EqualOP",
                     "GreatThanOP",
                     "LessThanOP",
                     "GreatEqualOP",
                     "LessEqualOP",
                     "LabelOP",
                     "FuncEndOP"};

int main() {
  AllInit();
  printf("开始遍历\n");

  return_val = (Value *)malloc(sizeof(Value));
  value_init(return_val);
  return_val->name = strdup("return_val");
  return_val->VTy->TID = DefaultTyID;

  // if嵌套的例子
  char *input1 =
      "int main() {int a;int b;int c;a = 5;b = 10; if(a==5){ a = "
      "30;b=40;c=a+b;if(b==20){c=19;a=233;}a=156; "
      "}else{ a = "
      "100; b=200;c=120+a;}int m=30,n=23;}";

  // 函数调用的例子
  char *input2 =
      "int binary_add(int x, int y) {"
      "  int z = x + y;"
      "  return z;"
      "}"
      "int main() {"
      "  int a;"
      "  int b;"
      "  int c;"
      "  a = 10;"
      "  b = 20;"
      "  c = binary_add(a,(b+c));"
      "  a = binary_add(c,b);"
      "}";

  parser(input2);

  printf("\npass the instruction list\n");
  void *element;
  ListFirst(ins_list, false);
  while (ListNext(ins_list, &element)) {
    // 打印出每条instruction的res的名字信息
    printf("opcode: %s    ", op_string[((Instruction *)element)->opcode]);
    printf("\tname: %s\n", ((Instruction *)element)->res->name);
  }

  printf("遍历结束\n\n");
  ListDeinit(ins_list);

  return 0;
}
