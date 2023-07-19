#ifndef AST_H
#define AST_H

#include "cds.h"
#include "instruction.h"
#include "symbol_table.h"
#include "value.h"

enum TAG { VARIABLETAG = 1, FUNCTIONTAG, CONSTNUMTAG, ARRAYTAG, STRUCTTAG };

extern int yylineno;        // 行号
extern char *yytext;        // 词
void yyerror(char *s, ...); // 错误处理函数

/*抽象语法树的结点*/
typedef struct _ast {
  int line;       // 行号
  char *name;     // 语法单元的名字
  struct _ast *l; // 左孩子
  struct _ast *r; // 右孩子
  union           // 共用体用来存放ID/TYPE/INTEGER/FLOAT结点的值
  {
    char *idtype;
    int intgr;
    float flt;
  };
} ast;

void eval_print(ast *a, int level);

/*构造抽象语法树,变长参数，name:语法单元名字；num:变长参数中语法结点个数*/
ast *newast(char *name, int num, ...);

void pre_eval(ast *);

void in_eval(ast *, Value *);

Value *post_eval(ast *, Value *, Value *);

/*遍历抽象语法树，level为树的层数*/
Value *eval(ast *);

#endif // LRPARSER_AST_H
