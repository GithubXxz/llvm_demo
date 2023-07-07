#include <stdarg.h> //变长参数函数所需的头文件
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
extern List *global_var_list;

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

  char *mix_feature =
      "int global_a = 10;"
      "int global_b = 20;"
      "int use_global(int a, int b, int arr[]) {"
      "return 2 * global_a + a * global_b + b * arr[3];"
      "}"
      "int main() {"
      "int a = 111;"
      "int b = 222;"
      "int arr[10][20];"
      "if (a == 111) {"
      "arr[3][3] = 333;"
      "b = 444;"
      "global_a = 555;"
      "} else {"
      "arr[3][3] = 666;"
      "global_a = 777;"
      "a = 999;"
      "}"
      "return a + global_a + 2 * use_global(b, global_b, arr[3]);"
      "}";

  char *logical_or = "int logical_or() {"
                     "  const int a = 233;"
                     "  const int b = 323;"
                     "  int res = 0;"
                     "  if (a > 200 || b > 500) {"
                     "    res = 10086;"
                     "  }else{"
                     "    res = 2333333;"
                     "  }"
                     "  return res;"
                     "}";

  if (freopen("printf_ast.txt", "w", stdout) == NULL) {
    fprintf(stderr, "打开文件失败！");
    exit(-1);
  }

#define PARSER
  parser(mix_feature);

#ifdef PARSER
  // 重定向输出回终端
  if (freopen(tty_path, "w", stdout) == NULL) {
    fprintf(stderr, "打开文件失败！");
    exit(-1);
  }

  if (freopen("out.txt", "w", stdout) == NULL) {
    fprintf(stderr, "打开文件失败！");
    exit(-1);
  }

  print_ins_pass(ins_list);

  // delete_return_deadcode_pass(ins_list);

  ins_toBBlock_pass(ins_list);

  print_ins_pass(global_var_list);

  ListFirst(func_list, false);
  void *element;
  while (ListNext(func_list, &element)) {
    puts(((Function *)element)->label->name);
    bblock_to_dom_graph_pass((Function *)element);
  }
#endif

  free(tty_path);
  printf("All over!\n");
  return 0;
}
