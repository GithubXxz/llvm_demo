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

int point_seed = 1;

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
      Value *else_label_ins = NULL;
      StackTop(stack_else_label, (void **)&else_label_ins);
      StackPop(stack_else_label);
      ListPushBack(ins_list, (void *)else_label_ins);
      printf("%s\n", else_label_ins->name);
    }
  }
}

void in_eval(ast *a, Value *left) {
  if (!strcmp(a->name, "FunDec")) {
    char temp_str[15];
    char text[10];
    sprintf(text, "%d", label_func_seed);
    ++label_func_seed;
    strcpy(temp_str, "func_label");
    strcat(temp_str, text);

    Value *func_label_ins = (Value *)ins_new_no_operator_v2(FuncLabelOP);
    // 添加变量的名字
    func_label_ins->name = strdup(temp_str);
    func_label_ins->VTy->TID = FuncLabelTyID;
    // 设定返回值类型
    func_label_ins->pdata->symtab_func_pdata.return_type = (int)nowVarDecType;
    func_label_ins->pdata->symtab_func_pdata.param_num = num_of_param;
    // 将参数的个数清零
    num_of_param = 0;

    // 插入
    ListPushBack(ins_list, (void *)func_label_ins);

    printf("%s\n", temp_str);

    // 将函数的<name,label>插入函数表
    HashMapPut(func_hashMap, strdup(a->l->idtype), func_label_ins);

    // 创建true条件下的label标签
    Value *entry_label_ins = (Value *)ins_new_no_operator_v2(LabelOP);

    // 添加变量的名字
    entry_label_ins->name = strdup("entry");
    entry_label_ins->VTy->TID = LabelTyID;

    ListPushBack(ins_list, entry_label_ins);

    printf("entry\n");
  }

  if (a->r && !strcmp(a->r->name, "assistIF")) {
    // 创建false条件下的label标签
    char temp_str[15];
    char text[10];
    sprintf(text, "%d", label_var_seed);
    ++label_var_seed;
    strcpy(temp_str, "label");
    strcat(temp_str, text);

    Value *else_label_ins = (Value *)ins_new_no_operator_v2(LabelOP);
    else_label_ins->name = strdup(temp_str);
    else_label_ins->VTy->TID = LabelTyID;

    // 将else-label入栈
    StackPush(stack_else_label, else_label_ins);

    // 创建true条件下的label标签
    sprintf(text, "%d", label_var_seed);
    ++label_var_seed;
    strcpy(temp_str, "label");
    strcat(temp_str, text);

    Value *true_label_ins = (Value *)ins_new_no_operator_v2(LabelOP);
    // 添加变量的名字
    true_label_ins->name = strdup(temp_str);
    true_label_ins->VTy->TID = LabelTyID;

    // 创建跳转语句
    Value *goto_condition_ins =
        (Value *)ins_new_single_operator_v2(GotoWithConditionOP, left);

    char temp_br_label_name[40];
    strcpy(temp_br_label_name, "true:");
    strcat(temp_br_label_name, true_label_ins->name);
    strcat(temp_br_label_name, "  false:");
    strcat(temp_br_label_name, else_label_ins->name);

    goto_condition_ins->name = strdup(temp_br_label_name);
    goto_condition_ins->VTy->TID = GotoTyID;
    goto_condition_ins->pdata->condition_goto.false_goto_location =
        else_label_ins;
    goto_condition_ins->pdata->condition_goto.true_goto_location =
        true_label_ins;

    ListPushBack(ins_list, (void *)goto_condition_ins);

    printf("br %s, true: %s  false : %s \n", left->name, true_label_ins->name,
           else_label_ins->name);

    ListPushBack(ins_list, true_label_ins);

    printf("%s\n1", temp_str);
  }

  if (a->r && !strcmp(a->r->name, "assistELSE")) {
    char temp_str[15];
    char text[10];
    sprintf(text, "%d", label_var_seed);
    ++label_var_seed;
    strcpy(temp_str, "label");
    strcat(temp_str, text);

    Value *then_label_ins = (Value *)ins_new_no_operator_v2(LabelOP);

    // 添加变量的名字
    then_label_ins->name = strdup(temp_str);
    then_label_ins->VTy->TID = LabelTyID;

    StackPush(stack_then_label, then_label_ins);

    // printf("new instruction destination %s and push to the then_stack\n",
    //        then_label->name);

    // cur_symboltable->symbol_map->put(cur_symboltable->symbol_map,
    //                                  strdup(temp_str), goto_else);

    Value *goto_else_ins = (Value *)ins_new_no_operator_v2(GotoOP);
    goto_else_ins->name = strdup("goto_then");
    goto_else_ins->VTy->TID = GotoTyID;
    goto_else_ins->pdata->no_condition_goto.goto_location = then_label_ins;

    ListPushBack(ins_list, (void *)goto_else_ins);

    printf("br %s \n", then_label_ins->name);
    ;
  }

  if (a->r && !strcmp(a->r->name, "assistArgs")) {
    char temp_str[15];
    char text[10];
    sprintf(text, "%d", param_seed);
    ++param_seed;
    strcpy(temp_str, "param");
    strcat(temp_str, text);

    Value *func_param_ins = (Value *)ins_new_no_operator_v2(ParamOP);

    // 添加变量的名字 类型 和返回值
    func_param_ins->name = strdup(temp_str);
    func_param_ins->VTy->TID = ParamTyID;
    func_param_ins->pdata->param_pdata.param_value = left;

    // 插入
    ListPushBack(ins_list, (void *)func_param_ins);

    printf("%s %s insert\n", func_param_ins->name, left->name);
  }
}

Value *post_eval(ast *a, Value *left, Value *right) {
  if (a != NULL) {
    // 如果要定义数据变量 判断当前定义的数据类型
    // 并且修改 NowVarDecType
    if (!strcmp(a->name, "TYPE")) {
      if (!strcmp(pre_astnode->name, "Specifire")) {
        if (!strcmp(a->idtype, NowVarDecStr[1]))
          nowVarDecType = NowInt;
        else if (!strcmp(a->idtype, NowVarDecStr[2]))
          nowVarDecType = NowFloat;
        else if (!strcmp(a->idtype, NowVarDecStr[3]))
          nowVarDecType = NowStruct;
      }
      return NULL;
    }

    // 判断父节点是不是变量声明 如果是 则创建一个该变量对应的value并返回
    if (!strcmp(pre_astnode->name, "VarDec")) {
      if (!strcmp(a->name, "ID")) {
        // 在内存中为变量分配空间
        Value *cur_var = (Value *)malloc(sizeof(Value));
        value_init(cur_var);
        // 添加变量类型
        cur_var->VTy->TID = (int)nowVarDecType;
        // 添加变量的名字
        cur_var->name = strdup(a->idtype);

        // 给指针变量命名
        char temp_str[15];
        char text[10];
        sprintf(text, "%d", point_seed);
        ++point_seed;
        strcpy(temp_str, "\%point");
        strcat(temp_str, text);

        // 创建指针
        Value *cur_ins = (Value *)ins_new_no_operator_v2(AllocateOP);
        // 添加变量类型
        cur_ins->VTy->TID = PointerTyID;
        // 添加指针的名字 映射进哈希表 放入symbol_tabel里面 用于索引
        cur_ins->name = strdup(temp_str);
        // 设定allocate语句的指针所指向的value*
        cur_ins->pdata->allocate_pdata.point_value = cur_var;

        printf("%s = alloca %s,align 4\n", temp_str,
               NowVarDecStr[nowVarDecType]);

        ListPushBack(ins_list, cur_ins);

        char *var_name = strdup(a->idtype);
        // 将变量加入符号表
        HashMapPut(cur_symboltable->symbol_map, var_name, cur_ins);
        // 返回指针
        return cur_ins;

      } else if  // 变量声明的时候同时初始化
          (!strcmp(a->name, "ASSIGNOP")) {
        return right;
      }
    }

    if (!strcmp(pre_astnode->name, "Exp")) {
      // 父节点是表达式的情况 且不是赋值表达式 且当前节点是ID的情况
      if (!strcmp(a->name, "ID") &&
          (pre_astnode->r ? strcmp(pre_astnode->r->name, "ASSIGNOP") : true)) {
        // 被引用变量的名字
        char *load_var_name = strdup(a->idtype);

        // 指向被引用变量的指针
        Value *load_var_pointer =
            HashMapGet(cur_symboltable->symbol_map, load_var_name);
        SymbolTable *pre_symboltable = cur_symboltable->father;
        // 查表 取出名字所指代的Value*
        while (!load_var_pointer) {
          // 如果当前表中没有且该表的父表为空 则报错语义错误
          // 向上一级查表
          load_var_pointer =
              HashMapGet(pre_symboltable->symbol_map, load_var_name);
          pre_symboltable = pre_symboltable->father;
          assert(pre_symboltable || load_var_pointer);
        };

        // 给引用变量命名
        char temp_str[15];
        char text[10];
        sprintf(text, "%d", temp_var_seed);
        ++temp_var_seed;
        strcpy(temp_str, "\%temp");
        strcat(temp_str, text);

        // 内容与指针所指向的pdata完全一样 名字不一样 占用的内存地址也不一样
        Value *load_ins =
            (Value *)ins_new_single_operator_v2(LoadOP, load_var_pointer);
        load_ins->name = strdup(temp_str);
        // 将内容拷贝
        value_copy(load_ins,
                   load_var_pointer->pdata->allocate_pdata.point_value);

        ListPushBack(ins_list, (void *)load_ins);

        printf("%s = load %s, %s,align 4\n", temp_str,
               NowVarDecStr[load_ins->VTy->TID < 4 ? load_ins->VTy->TID
                                                   : load_ins->VTy->TID - 4],
               load_var_pointer->name);

        return load_ins;
      } else if (!strcmp(a->name, "ID") && pre_astnode->r &&
                 !strcmp(pre_astnode->r->name, "ASSIGNOP")) {
        // 被赋值变量的名字
        char *assign_var_name = strdup(a->idtype);
        // 指向被赋值变量的指针
        Value *assign_var_pointer =
            HashMapGet(cur_symboltable->symbol_map, assign_var_name);
        SymbolTable *pre_symboltable = cur_symboltable->father;
        // 查表 取出名字所指代的Value*
        while (!assign_var_pointer) {
          // 如果当前表中没有且该表的父表为空 则报错语义错误
          // 向上一级查表
          assign_var_pointer =
              HashMapGet(pre_symboltable->symbol_map, assign_var_name);
          pre_symboltable = pre_symboltable->father;
          assert(pre_symboltable || assign_var_pointer);
        };
        return assign_var_pointer;
      } else if (!strcmp(a->name, "INTEGER")) {
        Value *cur = (Value *)malloc(sizeof(Value));
        value_init(cur);
        cur->VTy->TID = ImmediateIntTyID;
        // 添加变量的名字
        cur->name = strdup("immediateInt");
        // 为padata里的整数字面量常量赋值
        cur->pdata->var_pdata.iVal = a->intgr;
        return cur;
      } else if (!strcmp(a->name, "FLOAT")) {
        Value *cur = (Value *)malloc(sizeof(Value));
        value_init(cur);
        cur->VTy->TID = ImmediateFloatTyID;
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

        Value *call_fun_ins = (Value *)ins_new_no_operator_v2(CallOP);

        call_fun_ins->name = strdup("call_func");
        call_fun_ins->VTy->TID = FuncCallTyID;
        call_fun_ins->pdata->no_condition_goto.goto_location = func_label;

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
        return NULL;
      } else {
        if (a->r && !strcmp(a->r->name, "ASSIGNOP")) {
          // 把right存给left left是赋值号左边操作数的地址
          Value *store_ins =
              (Value *)ins_new_binary_operator_v2(StoreOP, left, right);
          ListPushBack(ins_list, (void *)store_ins);
          printf("store %s %s, %s,align 4\n",
                 NowVarDecStr[right->VTy->TID < 4 ? right->VTy->TID
                                                  : right->VTy->TID - 4],
                 right->name, var_name);
          return NULL;
        }
      }
    }

    // 判断当前节点是否为表达式节点
    if (!strcmp(a->name, "Exp")) {
      char *var_name = NULL;
      if (left) {
        var_name = strdup(left->name);
      }
      // else {
      //   printf("left name is null\n");
      // }

      if (right == NULL) {
        return left;
      } else if (!strcmp(a->r->name, "ASSIGNOP")) {
        // assign_var_pointer是赋值号左边操作数的地址
        Instruction *store_ins =
            ins_new_binary_operator_v2(StoreOP, left, right);
        ListPushBack(ins_list, (void *)store_ins);
        printf("store %s %s, %s,align 4\n",
               NowVarDecStr[right->VTy->TID < 4 ? right->VTy->TID
                                                : right->VTy->TID - 4],
               right->name, left->name);
        // TODO 返回值是什么有待考虑
        return right;
      } else {
        char temp_str[15];
        char text[10];
        sprintf(text, "%d", temp_var_seed);
        ++temp_var_seed;
        strcpy(temp_str, "\%temp");
        strcat(temp_str, text);

        // 放入符号表
        // HashMapPut(cur_symboltable->symbol_map, strdup(temp_str), cur);

        // printf("allocate storage for %s\n", temp_str);
        Value *cur_ins =
            (Value *)ins_new_binary_operator_v2(DefaultOP, left, right);
        // 添加变量的名字
        cur_ins->name = strdup(temp_str);
        cur_ins->VTy->TID = ins_res_type(left, right);

        char *ins_opcode =
            NowVarDecStr[cur_ins->VTy->TID < 4 ? cur_ins->VTy->TID
                                               : cur_ins->VTy->TID - 4];

        if (!strcmp(a->r->name, "PLUS")) {
          ((Instruction *)cur_ins)->opcode = AddOP;
          printf("%s = add nsw %s %s, %s\n", cur_ins->name, ins_opcode,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "MINUS")) {
          ((Instruction *)cur_ins)->opcode = SubOP;
          printf("%s = sub nsw %s %s, %s\n", cur_ins->name, ins_opcode,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "STAR")) {
          ((Instruction *)cur_ins)->opcode = MulOP;
          printf("%s = mul nsw %s %s, %s\n", cur_ins->name, ins_opcode,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "DIV")) {
          ((Instruction *)cur_ins)->opcode = DivOP;
          printf("%s = div nsw %s %s, %s\n", cur_ins->name, ins_opcode,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "EQUAL")) {
          ((Instruction *)cur_ins)->opcode = EqualOP;
          printf("%s = icmp eq %s %s, %s\n", cur_ins->name, ins_opcode,
                 left->name, right->name);
        }
        ListPushBack(ins_list, (void *)cur_ins);
        return cur_ins;
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
        // 删除链尾 并且释放链尾ins的内存
        ListPopBack(ins_list);
        value_free((Value *)ins_back);
        free(ins_back);

        // 释放内存
        Value *else_label_ins = NULL;
        StackTop(stack_else_label, (void **)&else_label_ins);
        StackPop(stack_else_label);
        ListPushBack(ins_list, (void *)else_label_ins);
        printf("%s\n", else_label_ins->name);

        Instruction *then_label_ins = NULL;
        StackTop(stack_then_label, (void **)&then_label_ins);
        StackPop(stack_then_label);
        // 释放label ins的内存
        // printf("delete the destination %s\n",
        // then_label_ins->user.res->name);
        value_free((Value *)then_label_ins);
        free(then_label_ins);
        return NULL;
      } else {
        // printf("this 'if' with else statement\n");
        Value *then_label_ins = NULL;
        StackTop(stack_then_label, (void **)&then_label_ins);
        StackPop(stack_then_label);
        ListPushBack(ins_list, (void *)then_label_ins);
        printf("%s\n", then_label_ins->name);
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

      char temp_str[20];
      char text[10];
      sprintf(text, "%d", label_func_seed - 1);
      strcpy(temp_str, "func_label");
      strcat(temp_str, text);
      strcat(temp_str, " end");

      Value *func_end_ins = (Value *)ins_new_no_operator_v2(FuncEndOP);
      // 添加变量的名字
      func_end_ins->name = strdup(temp_str);
      func_end_ins->VTy->TID = FuncEndTyID;
      // pdata不需要数据所以释放掉
      free(func_end_ins->pdata);

      // 插入
      ListPushBack(ins_list, (void *)func_end_ins);

      printf("%s\n", temp_str);
    }

    if (!strcmp(a->name, "RETURN")) {
      char temp_str[20];

      strcpy(temp_str, right->name);

      Value *func_return_ins = (Value *)ins_new_no_operator_v2(ReturnOP);
      // 添加变量的名字 类型 和返回值
      func_return_ins->name = strdup(temp_str);
      func_return_ins->VTy->TID = ReturnTyID;
      func_return_ins->pdata->return_pdata.return_value = right;

      // 插入
      ListPushBack(ins_list, (void *)func_return_ins);

      printf("%s\n", func_return_ins->name);
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
