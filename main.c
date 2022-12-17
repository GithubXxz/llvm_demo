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

ast *pre_astnode = NULL;

int yyparse(void);
int parser(char *input);

int main() {
  // 标准输出流重定向至stdout.txt流
  if (freopen("out.txt", "w", stdout) == NULL) {
    fprintf(stderr, "打开文件失败！");
    exit(-1);
  }
  AllInit();
  printf("开始遍历\n");
  char *input1 =
      "int main() {int a;int b;int c;a = 5;b = 10; if(a==5){ a = "
      "30;b=40;c=a+b;if(b==20){c=19;a=233;}a=156; "
      "}else{ a = "
      "100; b=200;c=120+a;}int m=30,n=23;}";

  char *input2 = "int inc(){int a;float b;a= a + 20;}";

  parser(input1);

  // void *element;
  // ListFirst(ins_list, false);
  // while (ListNext(ins_list, &element)) {
  //   // 打印出每条instruction的res的名字信息
  //   printf("%s\n", ((Instruction *)element)->res->name);
  // }

  printf("遍历结束\n\n");
  ListDeinit(ins_list);

  return 0;
}
