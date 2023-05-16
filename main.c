#include <stdarg.h>  //变长参数函数所需的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int yyparse(void);

int parser(char *input);

char *tty_path;

int main() {
  // 获取当前进程所在的终端设备路径
  tty_path = ttyname(STDIN_FILENO);

  AllInit();

  printf("开始遍历\n");

  char *multidimensional_arrays =
      "int main() {"
      "  int b = 10;"
      "  int c = 233,arr[10][20][30];"
      "  arr[3][5][b] = 100;"
      "  int d = arr[3][5][b];"
      "  return 0;"
      "}";

  char *func_call =
      "int add(int a, int b) {"
      "a = 19;"
      "int c = a + b;"
      "return c;"
      "}"
      "int main() {"
      "int a = 10;"
      "int b = 20;"
      "int c = add(a, b);"
      "return c;"
      "}";

  char *arr_func_call =
      "int add_arr(int arr[][20]) {"
      "int a = arr[1][2];"
      "arr[3][4] = 10;"
      "int b = arr[3][4];"
      "return a + b;"
      "}"
      "int main() {"
      "int arr[10][20];"
      "arr[1][2] = 10;"
      "arr[3][4] = 20;"
      "int res1 = add_arr(arr);"
      "return res1;"
      "}";

  if (freopen("printf_ast.txt", "w", stdout) == NULL) {
    fprintf(stderr, "打开文件失败！");
    exit(-1);
  }

  parser(arr_func_call);

  // 重定向输出回终端
  if (freopen(tty_path, "w", stdout) == NULL) {
    fprintf(stderr, "打开文件失败！");
    exit(-1);
  }

  print_ins_pass(ins_list);

  if (freopen("out.txt", "w", stdout) == NULL) {
    fprintf(stderr, "打开文件失败！");
    exit(-1);
  }

  // delete_return_deadcode_pass(ins_list);

  ins_toBBlock_pass(ins_list);

  ListFirst(func_list, false);
  void *element;
  while (ListNext(func_list, &element)) {
    puts(((Function *)element)->label->name);
    bblock_to_dom_graph_pass((Function *)element);
  }

  free(tty_path);
  printf("All over!\n");

  return 0;
}
