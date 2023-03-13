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

  char *input2 =
      "int main() {"
      "  int a = 10;"
      "  if (a == 10) {"
      "    a = 20;"
      "    if (a == 20) {"
      "      a = 30;"
      "    }"
      "  int b = a;"
      "  }"
      "  return 0;"
      "}";

  char *input3 =
      "int main() {"
      "  int a,b =10;"
      "  int c;"
      "  a = 10 + 15;"
      "  c = a + b + 3;"
      "}";

  char *input4 =
      "int main() {"
      "  int b,a;"
      "  b = 15;"
      "  if (b == 10) {"
      "    a = 333;"
      "  } else {"
      "    a = 444;"
      "  }"
      "  b = a;"
      "  return b;"
      "}";

  char *input5 =
      "int main() {"
      "  int a, b;"
      "  a = 10;"
      "  b = 120;"
      "  while (a < 5) {"
      "    a = a - 1;"
      "    if (b < 70) {"
      "      b = 100;"
      "      continue;"
      "    } else {"
      "      b = b -10;"
      "      break;"
      "    }"
      "  }"
      "  return b;"
      "}";

  char *input6 =
      "int main() {"
      "  int a = 10;"
      "  int b = 111;"
      "  int c = 222;"
      "  while (a > 5) {"
      "    int d = b;"
      "    b = c;"
      "    c = d;"
      "  }"
      "  return c;"
      "}";

  if (freopen("printf_ast.txt", "w", stdout) == NULL) {
    fprintf(stderr, "打开文件失败！");
    exit(-1);
  }

  parser(input6);

  //   print_ins_pass(ins_list);
  printf("遍历结束\n");

  //   if (freopen("out.txt", "w", stdout) == NULL) {
  //     fprintf(stderr, "打开文件失败！");
  //     exit(-1);
  //   }
  // 清除return语句和label或者functionEnd之间语句之间的不可达语句的pass
  print_ins_pass(ins_list);
  printf("\n");
  //   delete_return_deadcode_pass(ins_list);

  ins_toBBlock_pass(ins_list);

  bblock_to_dom_graph_pass(cur_func);

  //   ListDeinit(ins_list);

  //   use_relation_test();

  printf("All over!\n");

  return 0;
}
