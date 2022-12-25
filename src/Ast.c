#include "Ast.h"

#include <stdarg.h>  //变长参数函数所需的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Stack *stack_ast_pre;
extern Stack *stack_symbol_table;
extern Stack *stack_else_label;
extern Stack *stack_then_label;
extern ast *pre_astnode;
extern List *ins_list;
extern HashMap *func_hashMap;
extern SymbolTable *cur_symboltable;
extern Value *return_val;
void CleanObject(void *element);

enum NowVarDecType { NowInt = 1, NowFloat, NowStruct } nowVarDecType;

char *NowVarDecStr[] = {"default", "int", "float", "struct"};

int temp_var_seed = 1;  // 用于标识变量的名字

int label_var_seed = 1;  // 用于标识label的名字

int label_func_seed = 1;  // 用于表示func_label的名字

int param_seed = 1;

int num_of_param = 0;  // 用于标识函数参数的个数

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
    } else if (!strcmp(a->name, "FLOAT")) {
      a->flt = atoi(yytext);
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

void pre_eval(ast *a) {
  if (a != NULL) {
    if (!strcmp(a->name, "ParamDec")) {
      // 新建一个符号表用于存放参数
      ++num_of_param;
    }

    if (!strcmp(a->name, "FunDec")) {
      // 新建一个符号表用于存放参数
      cur_symboltable = (SymbolTable *)malloc(sizeof(SymbolTable));
      symbol_table_init(cur_symboltable);
    }

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
      StackTop(stack_symbol_table, (void **)&cur_symboltable);
    }

    if (!strcmp(a->name, "ELSE")) {
      Instruction *else_label_ins = NULL;
      StackTop(stack_else_label, (void **)&else_label_ins);
      StackPop(stack_else_label);
      ListPushBack(ins_list, (void *)else_label_ins);
      printf("%s\n", else_label_ins->user.res->name);
    }
  }
}

void in_eval(ast *a, Value *left) {
  if (!strcmp(a->name, "FunDec")) {
    Value *func_label = (Value *)malloc(sizeof(Value));
    value_init(func_label);
    char temp_str[15];
    char text[10];
    sprintf(text, "%d", label_func_seed);
    ++label_func_seed;
    strcpy(temp_str, "func_label");
    strcat(temp_str, text);

    // 添加变量的名字
    func_label->name = strdup(temp_str);
    func_label->VTy->TID = LabelTyID;
    // 设定返回值类型
    func_label->pdata->symtab_func_pdata.return_type = (int)nowVarDecType;
    func_label->pdata->symtab_func_pdata.param_num = num_of_param;
    // 将参数的个人清零
    num_of_param = 0;

    // cur_symboltable->symbol_map->put(cur_symboltable->symbol_map,
    //                                  strdup(temp_str), else_label);

    Instruction *func_label_ins = ins_new_no_operator(func_label, LabelOP);

    // 插入
    ListPushBack(ins_list, (void *)func_label_ins);

    // 将函数的<name,label>插入函数表
    HashMapPut(func_hashMap, strdup(a->l->idtype), func_label);

    printf("%s\n", temp_str);
  }

  if (a->r && !strcmp(a->r->name, "assistIF")) {
    Value *else_label = (Value *)malloc(sizeof(Value));
    value_init(else_label);
    char temp_str[15];
    char text[10];
    sprintf(text, "%d", label_var_seed);
    ++label_var_seed;
    strcpy(temp_str, "label");
    strcat(temp_str, text);

    // 添加变量的名字
    else_label->name = strdup(temp_str);
    else_label->VTy->TID = LabelTyID;

    // cur_symboltable->symbol_map->put(cur_symboltable->symbol_map,
    //                                  strdup(temp_str), else_label);

    Instruction *else_label_ins = ins_new_no_operator(else_label, LabelOP);

    StackPush(stack_else_label, else_label_ins);

    // printf("new instruction destination %s and push to the else_stack\n",
    //        else_label->name);

    Value *goto_else = (Value *)malloc(sizeof(Value));
    value_init(goto_else);

    goto_else->name = strdup("goto_else_or_then");
    goto_else->VTy->TID = GotoTyID;
    goto_else->pdata->instruction_pdata.goto_location = else_label;

    // cur_symboltable->symbol_map->put(cur_symboltable->symbol_map,
    //                                  strdup(temp_str), goto_else);

    Instruction *goto_else_ins =
        ins_new_single_operator(goto_else, GotoWithConditionOP, left);

    ListPushBack(ins_list, (void *)goto_else_ins);

    printf("new instruction goto %s with condition %s\n", else_label->name,
           left->name);

    // 创建true条件下的label标签
    Value *true_label = (Value *)malloc(sizeof(Value));
    value_init(true_label);

    sprintf(text, "%d", label_var_seed);
    ++label_var_seed;
    strcpy(temp_str, "label");
    strcat(temp_str, text);

    // 添加变量的名字
    true_label->name = strdup(temp_str);
    true_label->VTy->TID = LabelTyID;

    Instruction *true_label_ins = ins_new_no_operator(true_label, LabelOP);
    ListPushBack(ins_list, true_label_ins);

    printf("%s\n", temp_str);
  }

  if (a->r && !strcmp(a->r->name, "assistELSE")) {
    Value *then_label = (Value *)malloc(sizeof(Value));
    value_init(then_label);
    char temp_str[15];
    char text[10];
    sprintf(text, "%d", label_var_seed);
    ++label_var_seed;
    strcpy(temp_str, "label");
    strcat(temp_str, text);

    // 添加变量的名字
    then_label->name = strdup(temp_str);
    then_label->VTy->TID = LabelTyID;

    // cur_symboltable->symbol_map->put(cur_symboltable->symbol_map,
    //                                  strdup(temp_str), else_label);

    Instruction *then_label_ins = ins_new_no_operator(then_label, LabelOP);

    StackPush(stack_then_label, then_label_ins);

    // printf("new instruction destination %s and push to the then_stack\n",
    //        then_label->name);

    Value *goto_then = (Value *)malloc(sizeof(Value));
    value_init(goto_then);

    goto_then->name = strdup("goto_then");
    goto_then->VTy->TID = GotoTyID;
    goto_then->pdata->instruction_pdata.goto_location = then_label;

    // cur_symboltable->symbol_map->put(cur_symboltable->symbol_map,
    //                                  strdup(temp_str), goto_else);

    Instruction *goto_else_ins = ins_new_no_operator(goto_then, GotoOP);

    ListPushBack(ins_list, (void *)goto_else_ins);

    printf("new instruction goto %s without condition\n", then_label->name);
  }

  if (a->r && !strcmp(a->r->name, "assistArgs")) {
    Value *param = (Value *)malloc(sizeof(Value));
    value_init(param);

    char temp_str[15];
    char text[10];
    sprintf(text, "%d", param_seed);
    ++param_seed;
    strcpy(temp_str, "param");
    strcat(temp_str, text);

    // 添加变量的名字 类型 和返回值
    param->name = strdup(temp_str);
    param->VTy->TID = ParamTyID;
    param->pdata->param_pdata.param_value = left;

    Instruction *func_param_ins = ins_new_no_operator(param, ParamOP);

    // 插入
    ListPushBack(ins_list, (void *)func_param_ins);

    printf("%s %s insert\n", param->name, left->name);
  }
}

Value *post_eval(ast *a, Value *left, Value *right) {
  if (a != NULL) {
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
          // printf("successfully get %s\n", a->idtype);
          return cur;
        } else {
          // printf("cur symboltable can't find %s\n", a->idtype);
        }
        SymbolTable *pre_symboltable = cur_symboltable->father;
        // 查表 取出名字所指代的Value*
        while (!cur) {
          // 如果当前表中没有且该表的父表为空 则报错语义错误
          // 向上一级查表
          cur = HashMapGet(pre_symboltable->symbol_map, (void *)a->idtype);
          pre_symboltable = pre_symboltable->father;
          assert(pre_symboltable || cur);
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
               !strcmp(a->name, "ASSIGNOP") || !strcmp(a->name, "EQUAL")) {
        // 返回当前节点的右节点
        return right;
      } else if (!strcmp(a->name, "Stmt")) {
        // 返回当前节点的右节点
        return NULL;
      }
    }

    if (!strcmp(pre_astnode->name, "assistFuncCall")) {
      if (!strcmp(a->name, "ID")) {
        // 要跳转到的func_label
        Value *func_label = HashMapGet(func_hashMap, (void *)a->idtype);

        Value *call_fun = (Value *)malloc(sizeof(Value));
        value_init(call_fun);

        call_fun->name = strdup("call_func");
        call_fun->VTy->TID = FuncCallTyID;
        call_fun->pdata->instruction_pdata.goto_location = func_label;

        // cur_symboltable->symbol_map->put(cur_symboltable->symbol_map,
        //                                  strdup(temp_str), goto_else);

        Instruction *call_fun_ins = ins_new_no_operator(call_fun, CallOP);

        ListPushBack(ins_list, (void *)call_fun_ins);

        printf("new instruction call func %s and goto %s \n", a->idtype,
               func_label->name);
        // 修改返回参数的类型
        return_val->VTy->TID = func_label->pdata->symtab_func_pdata.return_type;
        return return_val;
      }
    }

    // 判断当前节点是否是变量声明 如果是则生成一条声明变量的instruction
    if (!strcmp(a->name, "VarDec")) {
      char *var_name = strdup(left->name);
      if (right == NULL) {
        // 将变量入加入哈希表
        HashMapPut(cur_symboltable->symbol_map, var_name, left);
        // printf("allocate storage for %s\n", var_name);
        return NULL;
      } else {
        if (!strcmp(a->r->name, "ASSIGNOP")) {
          // 将变量加入符号表
          HashMapPut(cur_symboltable->symbol_map, var_name, left);
          // 新建赋值语句
          Instruction *cur_ins = ins_new_single_operator(left, AssignOP, right);
          ListPushBack(ins_list, (void *)cur_ins);
          // printf("allocate storage for %s and %s = %s\n", var_name, var_name,
          //        right->name);
          printf("%s = %s\n", var_name, right->name);
          return NULL;
        }
      }
    }

    // 判断当前节点是否为表达式节点
    if (!strcmp(a->name, "Exp")) {
      char *var_name = NULL;
      if (left->name) {
        var_name = strdup(left->name);
      } else {
        printf("left name is null\n");
        exit(-1);
      }

      if (right == NULL) {
        return left;
      } else if (!strcmp(a->r->name, "ASSIGNOP")) {
        Instruction *cur_ins = ins_new_single_operator(left, AssignOP, right);
        ListPushBack(ins_list, (void *)cur_ins);
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
        // 放入符号表
        HashMapPut(cur_symboltable->symbol_map, strdup(temp_str), cur);

        // printf("allocate storage for %s\n", temp_str);

        if (!strcmp(a->r->name, "PLUS")) {
          Instruction *cur_ins =
              ins_new_binary_operator(cur, AddOP, left, right);
          ListPushBack(ins_list, (void *)cur_ins);
          printf("new instruction %s = %s + %s\n", cur->name, left->name,
                 right->name);
          return cur;
        } else if (!strcmp(a->r->name, "MINUS")) {
          Instruction *cur_ins =
              ins_new_binary_operator(cur, SubOP, left, right);
          ListPushBack(ins_list, (void *)cur_ins);
          printf(" new instruction %s = %s - %s\n", cur->name, left->name,
                 right->name);
          return cur;
        } else if (!strcmp(a->r->name, "STAR")) {
          Instruction *cur_ins =
              ins_new_binary_operator(cur, MulOP, left, right);
          ListPushBack(ins_list, (void *)cur_ins);
          printf(" new instruction %s = %s * %s\n", cur->name, left->name,
                 right->name);
          return cur;
        } else if (!strcmp(a->r->name, "DIV")) {
          Instruction *cur_ins =
              ins_new_binary_operator(cur, DivOP, left, right);
          ListPushBack(ins_list, (void *)cur_ins);
          printf(" new instruction %s = %s / %s\n", cur->name, left->name,
                 right->name);
          return cur;
        } else if (!strcmp(a->r->name, "EQUAL")) {
          Instruction *cur_ins =
              ins_new_binary_operator(cur, EqualOP, left, right);
          ListPushBack(ins_list, (void *)cur_ins);
          printf("new instruction %s = %s equal %s\n", cur->name, left->name,
                 right->name);
          return cur;
        }
      }
    }

    if (!strcmp(a->name, "assistFuncCall")) {
      param_seed = 1;
      return right;
    }

    // 后续遍历到if标识该if管辖的全区域块结束 插入跳转点label
    if (!strcmp(a->name, "IF")) {
      Instruction *ins_back = NULL;
      ListGetBack(ins_list, (void **)&ins_back);

      if (ins_back->opcode == GotoOP) {
        // printf(
        //     "this 'if' without else statement and delete goto destination "
        //     "label\n");
        // 删除链尾 并且释放链尾ins的内存
        ListPopBack(ins_list);
        value_free(ins_back->user.res);
        free(ins_back);

        // 释放内存
        Instruction *else_label_ins = NULL;
        StackTop(stack_else_label, (void **)&else_label_ins);
        StackPop(stack_else_label);
        ListPushBack(ins_list, (void *)else_label_ins);
        printf("%s\n", else_label_ins->user.res->name);

        Instruction *then_label_ins = NULL;
        StackTop(stack_then_label, (void **)&then_label_ins);
        StackPop(stack_then_label);
        // 释放label ins的内存
        // printf("delete the destination %s\n",
        // then_label_ins->user.res->name);
        value_free(then_label_ins->user.res);
        free(then_label_ins);
        return NULL;
      } else {
        // printf("this 'if' with else statement\n");
        Instruction *then_label_ins = NULL;
        StackTop(stack_then_label, (void **)&then_label_ins);
        StackPop(stack_then_label);
        ListPushBack(ins_list, (void *)then_label_ins);
        printf("%s\n", then_label_ins->user.res->name);
        return NULL;
      }
    }

    if (!strcmp(a->name, "FunDec")) {
      StackPop(stack_symbol_table);
      // 销毁当前的符号表中的哈希表然后销毁符号表
      HashMapDeinit(cur_symboltable->symbol_map);
      free(cur_symboltable);
      cur_symboltable = NULL;
      // 当前的符号表恢复到上一级的符号表
      // StackTop(stack_symbol_table, (void **)&cur_symboltable);

      Value *func_end = (Value *)malloc(sizeof(Value));
      value_init(func_end);
      char temp_str[20];
      char text[10];
      sprintf(text, "%d", label_func_seed - 1);
      strcpy(temp_str, "func_label");
      strcat(temp_str, text);
      strcat(temp_str, " end");

      // 添加变量的名字
      func_end->name = strdup(temp_str);
      func_end->VTy->TID = FuncEndTyID;
      // pdata不需要数据所以释放掉
      free(func_end->pdata);

      Instruction *func_end_ins = ins_new_no_operator(func_end, FuncEndOP);

      // 插入
      ListPushBack(ins_list, (void *)func_end_ins);

      printf("%s\n", temp_str);
    }

    if (!strcmp(a->name, "RETURN")) {
      Value *func_return = (Value *)malloc(sizeof(Value));
      value_init(func_return);

      char temp_str[20];

      strcpy(temp_str, "return ");
      strcat(temp_str, right->name);

      // 添加变量的名字 类型 和返回值
      func_return->name = strdup(temp_str);
      func_return->VTy->TID = ReturnTyID;
      func_return->pdata->return_pdata.return_value = right;

      Instruction *func_return_ins = ins_new_no_operator(func_return, ReturnOP);

      // 插入
      ListPushBack(ins_list, (void *)func_return_ins);

      printf("%s\n", func_return->name);
    }
  }
  return NULL;
}

Value *eval(ast *a) {
  if (a != NULL) {
    // 先序遍历
    pre_eval(a);
    // 将当前的ast节点如栈
    StackPush(stack_ast_pre, a);
    Value *left = eval(a->l);  // 遍历左子树
    // 中序遍历
    in_eval(a, left);

    Value *right = eval(a->r);  // 遍历右子树

    // 将当前的ast节点出栈
    StackPop(stack_ast_pre);
    // pre_astnode指向栈顶ast节点
    StackTop(stack_ast_pre, (void **)&pre_astnode);

    // 后序遍历
    return post_eval(a, left, right);
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
