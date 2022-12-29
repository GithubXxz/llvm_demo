#include <stdarg.h>  //变长参数函数所需的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Pass.h"
#include "bblock.h"
#include "c_container_auxiliary.h"
#include "cds.h"
#include "symbol_table.h"

extern List *ins_list;
extern List *func_list;

SymbolTable *cur_symboltable = NULL;

Value *return_val = NULL;

ast *pre_astnode = NULL;

BasicBlock *cur_bblock = NULL;

Function *cur_func = NULL;

int yyparse(void);
int parser(char *input);

int main() {
  AllInit();

  printf("开始遍历\n");

  return_val = (Value *)malloc(sizeof(Value));
  value_init(return_val);
  return_val->name = strdup("return_val");
  return_val->VTy->TID = DefaultTyID;

  // if嵌套的例子
  char *input1 =
      "int main() {"
      "  int a;"
      "  int b;"
      "  int c;"
      "  a = 5;"
      "  b = 10;"
      "  if (a == 5) {"
      "    a = 30;"
      "    b = 40;"
      "    c = a + b;"
      "    if (b == 20) {"
      "      c = 19;"
      "      a = 233;"
      "    }"
      "    a = 156;"
      "  } else {"
      "    a = 100;"
      "    b = 200;"
      "    c = 120 + a;"
      "  }"
      "  int m = 30, n = 23;"
      "  return a;"
      "}";

  // 函数调用的例子
  char *input2 =
      "int binary_add(int x, int y) {"
      "  int z = x + y;"
      "  return z;"
      "  z = 10 ;"
      "}"
      "int main() {"
      "  int a;"
      "  int b;"
      "  int c;"
      "  a = 10;"
      "  b = 20;"
      "  c = binary_add(a, (b + c));"
      "  return c;"
      "}";

  char *input3 =
      "int if_if_Else() {"
      "   int a;"
      "   a = 5;"
      "   int b;"
      "   b = 10;"
      "   if(a == 5){"
      "     if (b == 10)"
      "       a = 25;"
      "   }"
      "   else {"
      "     a = a + 15;"
      "   }"
      "   return (a);"
      " }"
      " int main(){"
      "   return (if_if_Else());"
      " }";

  // 模拟多个return的情况
  char *input4 =
      "int main() {"
      "int a = 55;"
      "int b = 46;"
      "if (a > 50) {"
      "return a;"
      "a = 111;"
      "a = 222;"
      "a = 333;"
      "} else {"
      "return b;"
      "b = 444;"
      "b = 555;"
      "}"
      "a = 666;"
      "a = 777;"
      "a = 888;"
      "return a;"
      "}";

  parser(input1);
  printf("遍历结束\n\n");

  // 清除return语句和label或者functionEnd之间语句之间的不可达语句的pass
  delete_return_deadcode_pass(ins_list);

  if (freopen("if-else.txt", "w", stdout) == NULL) {
    fprintf(stderr, "打开文件失败！");
    exit(-1);
  }

  // print_ins_pass(ins_list);

  //   printf("begin ins to block pass!\n");

  ins_toBBlock_pass(ins_list);

  //   printf("successfully ins to block pass!\n");

  print_bblock_pass(cur_func->entry_bblock);

  ListSetClean(ins_list, CleanObject);
  ListDeinit(ins_list);

  return 0;
}
