#include "Ast.h"

#include <stdarg.h>  //变长参数函数所需的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Stack *stack_ast_pre;
extern Stack *stack_symbol_table;
extern ast *pre_astnode;
extern List *ins_list;
void CleanObject(void *element);

extern SymbolTable *cur_symboltable;

enum NowVarDecType { NowInt = 1, NowFloat, NowStruct } nowVarDecType;

char *NowVarDecStr[] = {"default", "int", "float", "struct"};

int temp_var_seed = 1;

int i;

ast *newast(char *name, int num, ...)  // 抽象语法树建立
{
  va_list valist;                       // 定义变长参数列表
  ast *a = (ast *)malloc(sizeof(ast));  // 新生成的父节点
  a->l = NULL;
  a->r = NULL;
  ast *temp;  // 修改为局部指针而不分配所指向的内存空间
  if (!a) {
    yyerror("out of space");
    exit(0);
  }
  a->name = name;         // 语法单元名字
  va_start(valist, num);  // 初始化变长参数为num后的参数

  if (num > 0)  // num>0为非终结符：变长参数均为语法树结点，孩子兄弟表示法
  {
    temp = va_arg(valist,
                  ast *);  // 取变长参数列表中的第一个结点设为a的左孩子
    a->l = temp;
    a->line = temp->line;  // 父节点a的行号等于左孩子的行号

    if (num >= 2)  // 可以规约到a的语法单元>=2
    {
      for (i = 0; i < num - 1;
           ++i)  // 取变长参数列表中的剩余结点，依次设置成兄弟结点
      {
        temp->r = va_arg(valist, ast *);
        temp = temp->r;
      }
      temp->r = NULL;
    }
  } else  // num==0为终结符或产生空的语法单元：第1个变长参数表示行号，产生空的语法单元行号为-1。
  {
    int t = va_arg(valist, int);  // 取第1个变长参数
    a->line = t;
    if ((!strcmp(a->name, "ID")) ||
        (!strcmp(a->name, "TYPE")))  //"ID,TYPE,INTEGER，借助union保存yytext的值
    {
      char *t;
      t = (char *)malloc(sizeof(char *) * 40);
      strcpy(t, yytext);
      a->idtype = t;
    } else if (!strcmp(a->name, "INTEGER")) {
      a->intgr = atoi(yytext);
    } else {
    }
  }
  return a;
}

void eval_print(ast *a, int level) {
  // 打印该节点
  if (a != NULL) {
    for (i = 0; i < level; ++i)  // 孩子结点相对父节点缩进2个空格
      printf("  ");
    if (a->line != -1) {  // 产生空的语法单元不需要打印信息
      printf("%s ",
             a->name);  // 打印语法单元名字，ID/TYPE/INTEGER要打印yytext的值
      if ((!strcmp(a->name, "ID")) || (!strcmp(a->name, "TYPE")))
        printf(":%s ", a->idtype);
      else if (!strcmp(a->name, "INTEGER"))
        printf(":%d ", a->intgr);
      else
        printf("(%d)", a->line);
    } else {
      printf("%s ",
             a->name);  // 打印语法单元名字，ID/TYPE/INTEGER要打印yytext的值
    }

    // if (!strcmp(a->name, "SEMI")) {
    //   printf("     free a node\n");
    //   free(a);
    //   a = NULL;
    //   return NULL;
    // }
    printf("\n");

    eval_print(a->l, level + 1);  // 遍历左子树
    eval_print(a->r, level);      // 遍历右子树
  }
}

Value *eval(ast *a, int level) {
  if (a != NULL) {
    // if (!strcmp(a->name, "SEMI")) {
    //   printf("free a node\n");
    //   free(a);
    //   a = NULL;
    //   return NULL;
    // }

    if (!strcmp(a->name, "LC")) {
      cur_symboltable = (SymbolTable *)malloc(sizeof(SymbolTable));
      symbol_table_init(cur_symboltable);
    }

    if (!strcmp(a->name, "RC")) {
      // printf("cur_symboltable->symbol_map contains Value's name %s\n",
      //        ((Value *)cur_symboltable->symbol_map->get(
      //             cur_symboltable->symbol_map, "a"))
      //            ->name);
      StackPop(stack_symbol_table);
      // 销毁当前的符号表中的哈希表然后销毁符号表
      HashMapDeinit(cur_symboltable->symbol_map);
      free(cur_symboltable);
      cur_symboltable = NULL;
      // 当前的符号表恢复到上一级的符号表
      StackTop(stack_symbol_table, &cur_symboltable);
    }

    // 将当前的ast节点如栈
    StackPush(stack_ast_pre, a);
    Value *left = eval(a->l, level + 1);  // 遍历左子树
    Value *right = eval(a->r, level);     // 遍历右子树

    // 将当前的ast节点出栈
    StackPop(stack_ast_pre);

    StackTop(stack_ast_pre, (void **)&pre_astnode);
    // 如果要定义数据变量 判断当前定义的数据类型
    // 并且修改 NowVarDecType
    if (!strcmp(a->name, "TYPE")) {
      if (!strcmp(pre_astnode->name, "Specifire")) {
        if (!strcmp(a->idtype, NowVarDecStr[0]))
          nowVarDecType = NowInt;
        else if (!strcmp(a->idtype, NowVarDecStr[1]))
          nowVarDecType = NowFloat;
        else if (!strcmp(a->idtype, NowVarDecStr[2]))
          nowVarDecType = NowStruct;
      }
      return NULL;
    }

    // 判断父节点是不是变量声明 如果是 则创建一个该变量对应的value并返回
    if (!strcmp(pre_astnode->name, "VarDec")) {
      // if (a->l != NULL) {
      //   printf("new instruction: %s = 0 type: %s\n", a->l->idtype,
      //          NowVarDecStr[nowVarDecType]);
      // }
      if (!strcmp(a->name, "ID")) {
        Value *cur = (Value *)malloc(sizeof(Value));
        value_init(cur);
        // 添加变量类型
        cur->VTy->TID = (int)nowVarDecType;
        // 添加变量的名字
        cur->name = strdup(a->idtype);
        return cur;
      } else if  // 变量声明的时候同时初始化
          (!strcmp(a->name, "ASSIGNOP")) {
        return right;
      }
    }

    // 父节点是表达式的情况
    if (!strcmp(pre_astnode->name, "Exp")) {
      // 父节点是表达式当前节点是ID的情况
      if (!strcmp(a->name, "ID")) {
        Value *cur = HashMapGet(cur_symboltable->symbol_map, (void *)a->idtype);
        if (cur) {
          printf("successfully get %s\n", a->idtype);
          return cur;
        } else {
          printf("cur symboltable can't find %s\n", a->idtype);
        }

        SymbolTable *pre_symboltable = cur_symboltable->father;
        // 查表 取出名字所指代的Value*
        while (!cur) {
          // 如果当前表中没有且该表的父表为空 则报错语义错误
          assert(pre_symboltable->father == NULL);
          // 向上一级查表
          cur = HashMapGet(pre_symboltable->symbol_map, (void *)a->idtype);
          pre_symboltable = pre_symboltable->father;
        };
        return cur;
      } else if (!strcmp(a->name, "INTEGER")) {
        Value *cur = (Value *)malloc(sizeof(Value));
        value_init(cur);
        cur->VTy->TID = ConstIntTyID;
        // 添加变量的名字
        cur->name = strdup("immediateInt");
        // 为padata里的整数字面量常量赋值
        cur->pdata->var_pdata.iVal = a->intgr;
        return cur;
      } else if (!strcmp(a->name, "FLOAT")) {
        Value *cur = (Value *)malloc(sizeof(Value));
        value_init(cur);
        cur->VTy->TID = ConstFloatTyID;
        // 添加变量的名字
        cur->name = strdup("immediateFloat");
        // 为padata里的浮点数字面量常量赋值
        cur->pdata->var_pdata.fVal = a->flt;
        return cur;
      }
      // 加减乘除的情况
      else if (!strcmp(a->name, "MINUS") || !strcmp(a->name, "PLUS") ||
               !strcmp(a->name, "STAR") || !strcmp(a->name, "DIV") ||
               !strcmp(a->name, "ASSIGNOP")) {
        // 返回当前节点的右节点
        return right;
      }
    }

    // 判断当前节点是否是变量声明 如果是则生成一条声明变量的instruction
    if (!strcmp(a->name, "VarDec")) {
      char *var_name = strdup(left->name);
      if (right == NULL) {
        // 将a入加入哈希表
        HashMapPut(cur_symboltable->symbol_map, var_name, left);
        printf("allocate storage for %s\n", var_name);
        return NULL;
      } else {
        if (!strcmp(a->r->name, "ASSIGNOP")) {
          Instruction *cur_inst =
              ins_new_single_operator(left, AssignOP, right);
          cur_symboltable->symbol_map->put(cur_symboltable->symbol_map,
                                           var_name, left);
          ListPushBack(ins_list, (void *)cur_inst);
          printf("allocate storage for %s and %s = %s\n", var_name, var_name,
                 right->name);
          return NULL;
        }
      }
    }

    // 判断当前节点是否为表达式节点
    if (!strcmp(a->name, "Exp")) {
      char *var_name = strdup(left->name);
      if (right == NULL) {
        return left;
      } else if (!strcmp(a->r->name, "ASSIGNOP")) {
        Instruction *cur_inst = ins_new_single_operator(left, AssignOP, right);
        ListPushBack(ins_list, (void *)cur_inst);
        printf("new instruction %s = %s\n", var_name, right->name);
        return left;
      } else {
        Value *cur = (Value *)malloc(sizeof(Value));
        value_init(cur);
        char temp_str[15];
        char text[10];
        sprintf(text, "%d", temp_var_seed);
        ++temp_var_seed;
        strcpy(temp_str, "temp");
        strcat(temp_str, text);

        // 添加变量的名字
        cur->name = strdup(temp_str);
        cur->VTy->TID = ins_res_type(left, right);

        cur_symboltable->symbol_map->put(cur_symboltable->symbol_map,
                                         strdup(temp_str), cur);
        printf("allocate storage for %s\n", temp_str);

        if (!strcmp(a->r->name, "PLUS")) {
          Instruction *cur_inst =
              ins_new_binary_operator(cur, AddOP, left, right);
          ListPushBack(ins_list, (void *)cur_inst);
          // 新建一个判断左值类型的函数
          printf("new instruction %s = %s + %s\n", cur->name, left->name,
                 right->name);
          return cur;
        } else if (!strcmp(a->r->name, "MINUS")) {
          Instruction *cur_inst =
              ins_new_binary_operator(cur, SubOP, left, right);
          ListPushBack(ins_list, (void *)cur_inst);
          printf(" new instruction %s = %s - %s\n", cur->name, left->name,
                 right->name);
          return left;
        } else if (!strcmp(a->r->name, "STAR")) {
          Instruction *cur_inst =
              ins_new_binary_operator(cur, MulOP, left, right);
          ListPushBack(ins_list, (void *)cur_inst);
          printf(" new instruction %s = %s * %s\n", cur->name, left->name,
                 right->name);
          return left;
        } else if (!strcmp(a->r->name, "DIV")) {
          Instruction *cur_inst =
              ins_new_binary_operator(cur, DivOP, left, right);
          ListPushBack(ins_list, (void *)cur_inst);
          printf(" new instruction %s = %s / %s\n", cur->name, left->name,
                 right->name);
          return left;
        }
      }
    }
  }

  return NULL;
}

void yyerror(char *s, ...)  // 变长参数错误处理函数
{
  va_list ap;
  va_start(ap, s);
  fprintf(stderr, "%d:error:", yylineno);  // 错误行号
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}
