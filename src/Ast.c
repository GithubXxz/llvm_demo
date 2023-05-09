#include "Ast.h"

#include <stdarg.h>  //变长参数函数所需的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Stack *stack_ast_pre;
extern Stack *stack_symbol_table;
extern Stack *stack_else_label;
extern Stack *stack_then_label;
extern Stack *stack_while_head_label;  // while循环头(条件判断)
extern Stack *stack_while_then_label;  // while条件为false时所要跳转的label栈
extern ast *pre_astnode;
extern List *ins_list;
extern HashMap *func_hashMap;
extern SymbolTable *cur_symboltable;

void CleanObject(void *element);

enum NowVarDecType { NowVoid, NowInt, NowFloat, NowStruct } nowVarDecType;

char *NowVarDecStr[] = {"void", "int", "float", "struct"};

static int temp_var_seed = 1;    // 用于标识变量的名字
static int label_var_seed = 1;   // 用于标识label的名字
static int label_func_seed = 1;  // 用于表示func_label的名字
static int param_seed = 1;       // 用于表示函数参数的变量名
static int point_seed = 1;       // 用于表示指针变量名 用于alloca
static int array_seed = 1;       // 用于表示数组变量名 用于alloca
static int num_of_param = 0;     // 用于标识函数参数的个数
static int total_array_member = 1;
static List *array_list = NULL;
static Value *cur_construction_func;

// 判断当前if是否含有else
bool have_else = false;

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
      for (int i = 0; i < num - 1;
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
    for (int i = 0; i < level; ++i)  // 孩子结点相对父节点缩进2个空格
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
    if (!strcmp(a->name, "FunDec")) {
      // 新建一个符号表用于存放参数
      cur_symboltable = (SymbolTable *)malloc(sizeof(SymbolTable));
      symbol_table_init(cur_symboltable);

      Value *func_label_ins = (Value *)ins_new_no_operator_v2(FuncLabelOP);
      // 添加变量的名字
      func_label_ins->name = strdup(a->l->idtype);
      func_label_ins->VTy->TID = FuncLabelTyID;
      func_label_ins->pdata->symtab_func_pdata.return_type = (int)nowVarDecType;
      cur_construction_func = func_label_ins;

      // 插入
      ListPushBack(ins_list, (void *)func_label_ins);

      printf("Func: %s\n", a->l->idtype);

      // 将函数的<name,label>插入函数表
      HashMapPut(func_hashMap, strdup(a->l->idtype), func_label_ins);

      Value *entry_label_ins = (Value *)ins_new_no_operator_v2(LabelOP);

      // 添加变量的名字
      entry_label_ins->name = strdup("entry");
      entry_label_ins->VTy->TID = LabelTyID;

      ListPushBack(ins_list, entry_label_ins);

      // printf("entry\n");
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

    // 全局变量的符号表
    if (!strcmp(a->name, "ExtDecList") && StackSize(stack_symbol_table) == 0) {
      cur_symboltable = (SymbolTable *)malloc(sizeof(SymbolTable));
      symbol_table_init(cur_symboltable);
    }

    if (a->r && !strcmp(a->r->name, "assistWHILE")) {
      // 创建while这条语句的label，用于返回循环头
      char temp_str[30];
      char text[10];
      sprintf(text, "%d", label_var_seed);
      ++label_var_seed;
      strcpy(temp_str, "label");
      strcat(temp_str, text);

      Value *while_head_label_ins = (Value *)ins_new_no_operator_v2(LabelOP);
      while_head_label_ins->name = strdup(temp_str);
      while_head_label_ins->VTy->TID = LabelTyID;
      StackPush(stack_while_head_label, while_head_label_ins);

      // 创建llvm 模式goto语句
      Value *goto_label_ins = (Value *)ins_new_no_operator_v2(GotoOP);
      strcpy(temp_str, "goto ");
      strcat(temp_str, while_head_label_ins->name);
      goto_label_ins->name = strdup(temp_str);
      goto_label_ins->VTy->TID = GotoTyID;
      goto_label_ins->pdata->no_condition_goto.goto_location =
          while_head_label_ins;
      printf("br %s\n", while_head_label_ins->name);
      printf("%s\n", while_head_label_ins->name);
      // 插入跳转语句
      ListPushBack(ins_list, goto_label_ins);
      // 插入while循环头的label
      ListPushBack(ins_list, while_head_label_ins);
    }
  }
}

void in_eval(ast *a, Value *left) {
  if (!strcmp(a->name, "ParamDec")) {
    // 新建一个符号表用于存放参数
    // 在内存中为变量分配空间
    Value *cur_var = (Value *)malloc(sizeof(Value));
    value_init(cur_var);
    // 添加变量类型
    cur_var->VTy->TID = (int)nowVarDecType;
    char param_name[30];
    sprintf(param_name, "param%d", num_of_param);
    // 添加变量的名字
    cur_var->name = strdup(param_name);
    // 返回指针
    Value *store_ins =
        (Value *)ins_new_binary_operator_v2(InitArgOP, left, cur_var);
    store_ins->IsInitArgs = 1;
    store_ins->pdata->param_init_pdata.the_param_index = num_of_param++;
    printf("store %s %s, %s,align 4\n",
           NowVarDecStr[cur_var->VTy->TID < 4 ? cur_var->VTy->TID
                                              : cur_var->VTy->TID - 4],
           cur_var->name, left->name);
    ListPushBack(ins_list, store_ins);
    return;
  }

  if (!strcmp(a->name, "FunDec")) {
    cur_construction_func->pdata->symtab_func_pdata.param_num = num_of_param;
    // 将参数的个数清零
    num_of_param = 0;
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

    // 创建跳转语句 left是跳转条件
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

    printf("%s\n", temp_str);
  }

  if (a->r && !strcmp(a->r->name, "assistELSE")) {
    char temp_str[30];
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
    strcpy(temp_str, "goto ");
    strcat(temp_str, then_label_ins->name);
    goto_else_ins->name = strdup(temp_str);
    goto_else_ins->VTy->TID = GotoTyID;
    goto_else_ins->pdata->no_condition_goto.goto_location = then_label_ins;

    ListPushBack(ins_list, (void *)goto_else_ins);

    printf("br %s \n", then_label_ins->name);
  }

  // args_insert
  if (a->r && !strcmp(a->r->name, "assistArgs")) {
    char temp_str[30];
    char text[10];
    sprintf(text, "%d", param_seed);
    ++param_seed;
    strcpy(temp_str, "param_convert");
    strcat(temp_str, text);

    Value *func_param_ins = (Value *)ins_new_single_operator_v2(ParamOP, left);

    // 添加变量的名字 类型 和返回值
    func_param_ins->name = strdup(temp_str);
    func_param_ins->VTy->TID = ParamTyID;
    // func_param_ins->pdata->param_pdata.param_value = left;

    // 插入
    ListPushBack(ins_list, (void *)func_param_ins);

    printf("%s %s insert\n", func_param_ins->name, left->name);
  }

  if (a->r && !strcmp(a->r->name, "assistWHILE")) {
    char temp_str[30];
    char text[10];
    // 创建false条件下的label标签
    sprintf(text, "%d", label_var_seed);
    ++label_var_seed;
    strcpy(temp_str, "label");
    strcat(temp_str, text);

    Value *while_false_label_ins = (Value *)ins_new_no_operator_v2(LabelOP);
    while_false_label_ins->name = strdup(temp_str);
    while_false_label_ins->VTy->TID = LabelTyID;
    // 将 while_then_label入栈
    StackPush(stack_while_then_label, while_false_label_ins);

    // 创建true条件下的label标签
    sprintf(text, "%d", label_var_seed);
    ++label_var_seed;
    strcpy(temp_str, "label");
    strcat(temp_str, text);

    Value *while_true_label_ins = (Value *)ins_new_no_operator_v2(LabelOP);
    // 添加变量的名字
    while_true_label_ins->name = strdup(temp_str);
    while_true_label_ins->VTy->TID = LabelTyID;

    // 创建跳转语句
    Value *goto_condition_ins =
        (Value *)ins_new_single_operator_v2(GotoWithConditionOP, left);

    char temp_br_label_name[40];
    strcpy(temp_br_label_name, "true:");
    strcat(temp_br_label_name, while_true_label_ins->name);
    strcat(temp_br_label_name, "  false:");
    strcat(temp_br_label_name, while_false_label_ins->name);

    goto_condition_ins->name = strdup(temp_br_label_name);
    goto_condition_ins->VTy->TID = GotoTyID;
    goto_condition_ins->pdata->condition_goto.false_goto_location =
        while_false_label_ins;
    goto_condition_ins->pdata->condition_goto.true_goto_location =
        while_true_label_ins;

    ListPushBack(ins_list, (void *)goto_condition_ins);

    printf("while br %s, true: %s  false : %s \n", left->name,
           while_true_label_ins->name, while_false_label_ins->name);

    ListPushBack(ins_list, while_true_label_ins);

    printf("%s\n", temp_str);
  }
}

Value *post_eval(ast *a, Value *left, Value *right) {
  if (a != NULL) {
    // 如果要定义数据变量 判断当前定义的数据类型
    // 并且修改 NowVarDecType
    if (!strcmp(a->name, "TYPE")) {
      if (!strcmp(pre_astnode->name, "Specifire")) {
        if (!strcmp(a->idtype, NowVarDecStr[0])) {
          nowVarDecType = NowVoid;
        } else if (!strcmp(a->idtype, NowVarDecStr[1])) {
          nowVarDecType = NowInt;
        } else if (!strcmp(a->idtype, NowVarDecStr[2]))
          nowVarDecType = NowFloat;
        else if (!strcmp(a->idtype, NowVarDecStr[3]))
          nowVarDecType = NowStruct;
      }
    }

    if (!strcmp(a->name, "Specifire")) {
      return right;
    }

    // 判断父节点是不是变量声明 如果是 则创建一个该变量对应的value并返回
    // pre_var_dec
    if (!strcmp(pre_astnode->name, "VarDec")) {
      if (!strcmp(a->name, "ID")) {
        // allocate for array
        if (pre_astnode->r && !strcmp(pre_astnode->r->name, "LB")) {
          // 给指针变量命名
          char temp_str[15];
          char text[10];
          sprintf(text, "%d", array_seed);
          ++array_seed;
          strcpy(temp_str, "\%array");
          strcat(temp_str, text);

          // 创建指针
          Value *cur_ins = (Value *)ins_new_no_operator_v2(AllocateOP);
          // 添加变量类型
          cur_ins->VTy->TID = ArrayTyID;
          // 添加指针的名字 映射进哈希表 放入symbol_tabel里面 用于索引
          cur_ins->name = strdup(temp_str);

          array_list = ListInit();
          ListSetClean(array_list, CleanObject);

          printf("%s = alloca %s array,align 4\n", temp_str,
                 NowVarDecStr[nowVarDecType]);

          ListPushBack(ins_list, cur_ins);

          char *var_name = strdup(a->idtype);
          // 将变量加入符号表
          HashMapPut(cur_symboltable->symbol_map, var_name, cur_ins);
          // 返回指针
          return cur_ins;
        } else {
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
        }
      } else if  // 变量声明的时候同时初始化
          (!strcmp(a->name, "ASSIGNOP")) {
        return right;
      } else if  // 变量声明的时候同时初始化
          (!strcmp(a->name, "LB")) {
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
        if (load_var_pointer->VTy->TID == ArrayTyID) {
          return load_var_pointer;
        } else {
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
        }
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
        char text[10];
        sprintf(text, "%d", a->intgr);
        // 添加变量的名字
        cur->name = strdup(text);
        // 为padata里的整数字面量常量赋值
        cur->pdata->var_pdata.iVal = a->intgr;
        return cur;
      } else if (!strcmp(a->name, "FLOAT")) {
        Value *cur = (Value *)malloc(sizeof(Value));
        value_init(cur);
        cur->VTy->TID = ImmediateFloatTyID;
        char text[10];
        sprintf(text, "%f", a->flt);
        // 添加变量的名字
        cur->name = strdup(text);
        // 为padata里的浮点数字面量常量赋值
        cur->pdata->var_pdata.fVal = a->flt;
        return cur;
      }
      // 加减乘除的情况
      else if (!strcmp(a->name, "MINUS") || !strcmp(a->name, "PLUS") ||
               !strcmp(a->name, "STAR") || !strcmp(a->name, "DIV") ||
               !strcmp(a->name, "ASSIGNOP") || !strcmp(a->name, "EQUAL") ||
               !strcmp(a->name, "NOTEQUAL") || !strcmp(a->name, "GREAT") ||
               !strcmp(a->name, "GREATEQUAL") ||
               !strcmp(a->name, "LESSEQUAL") || !strcmp(a->name, "LESS") ||
               !strcmp(a->name, "LB")) {
        // 返回当前节点的右节点
        return right;
      } else if (!strcmp(a->name, "Stmt")) {
        // 返回当前节点的右节点
        return NULL;
      }
    }

    // funccall
    if (!strcmp(pre_astnode->name, "assistFuncCall")) {
      if (!strcmp(a->name, "ID")) {
        // 要跳转到的func_label
        Value *func_label = HashMapGet(func_hashMap, (void *)a->idtype);

        if (func_label->pdata->symtab_func_pdata.return_type == VoidTyID) {
          Value *call_fun_ins = (Value *)ins_new_no_operator_v2(CallOP);
          call_fun_ins->pdata->func_call_pdata.name = strdup(a->idtype);

          call_fun_ins->name = strdup("func_call");
          call_fun_ins->VTy->TID =
              func_label->pdata->symtab_func_pdata.return_type;

          ListPushBack(ins_list, (void *)call_fun_ins);

          printf(
              "new instruction call func %s and goto %s without return value "
              "\n",
              a->idtype, func_label->name);

          return NULL;
        } else {
          Value *call_fun_ins =
              (Value *)ins_new_no_operator_v2(CallWithReturnValueOP);
          call_fun_ins->pdata->func_call_pdata.name = strdup(a->idtype);

          // oprand the array
          char temp_str[15];
          char text[10];
          sprintf(text, "%d", temp_var_seed);
          ++temp_var_seed;
          strcpy(temp_str, "\%temp");
          strcat(temp_str, text);

          call_fun_ins->name = strdup(temp_str);
          call_fun_ins->VTy->TID =
              func_label->pdata->symtab_func_pdata.return_type;

          ListPushBack(ins_list, (void *)call_fun_ins);

          printf(
              "new instruction call func %s and goto %s with return value \n",
              a->idtype, func_label->name);

          return call_fun_ins;
        }
      }
    }

    // 判断当前节点是否是变量声明 如果是则生成一条声明变量的instruction
    if (!strcmp(a->name, "VarDec")) {
      char *var_name = strdup(left->name);
      if (left->VTy->TID != ArrayTyID) {
        if (right == NULL) {
          return left;
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
      } else {
        // init the array
        if (a->r && !strcmp(a->r->name, "LB")) {
          total_array_member *= right->pdata->var_pdata.iVal;
          // printf("total array member multiple %d\n",
          //        right->pdata->var_pdata.iVal);
          ListPushBack(array_list,
                       (void *)(intptr_t)(right->pdata->var_pdata.iVal));
        } else if (a->r && !strcmp(a->r->name, "ASSIGNOP")) {
        } else {
          // printf("%s have %d members\n", left->name, total_array_member);
          left->pdata->array_pdata.list_para = array_list;
          left->pdata->array_pdata.total_member = total_array_member;
          left->pdata->array_pdata.array_value =
              malloc(sizeof(Value) * total_array_member);
          for (int ii = 0; ii < total_array_member; ii++) {
            // 在内存中为变量分配空间
            Value *cur_var = (Value *)malloc(sizeof(Value));
            value_init(cur_var);
            // 添加变量类型
            cur_var->VTy->TID = (int)nowVarDecType;

            // 创建指针
            Value *cur_ins = &(left->pdata->array_pdata.array_value[ii]);
            value_init(cur_ins);
            cur_ins->name = strdup("array_memeber");
            // 添加变量类型
            cur_ins->VTy->TID = PointerTyID;
            // 设定allocate语句的指针所指向的value*
            cur_ins->pdata->allocate_pdata.point_value = cur_var;
          }
          total_array_member = 1;
          array_list = NULL;
        }
        return left;
      }
    }

    // 判断当前节点是否为表达式节点
    if (!strcmp(a->name, "Exp")) {
      char *var_name = NULL;
      if (left) {
        var_name = left->name;
      }

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
      } else if (!strcmp(a->r->name, "LB")) {
        // oprand the array
        char temp_str[15];
        char text[10];
        sprintf(text, "%d", temp_var_seed);
        ++temp_var_seed;
        strcpy(temp_str, "\%temp");
        strcat(temp_str, text);
        Value *cur_ins =
            (Value *)ins_new_binary_operator_v2(GetelementptrOP, left, right);
        // 将数组的信息拷贝一份
        cur_ins->name = strdup(temp_str);
        cur_ins->VTy->TID = ArrayTyID;
        cur_ins->pdata->array_pdata.array_value =
            left->pdata->array_pdata.array_value;
        cur_ins->pdata->array_pdata.list_para = ListInit();
        ListSetClean(cur_ins->pdata->array_pdata.list_para, CleanObject);
        list_copy(cur_ins->pdata->array_pdata.list_para,
                  left->pdata->array_pdata.list_para);
        cur_ins->pdata->array_pdata.total_member =
            left->pdata->array_pdata.total_member;
        void *element;
        ListGetFront(cur_ins->pdata->array_pdata.list_para, &element);
        ListPopFront(cur_ins->pdata->array_pdata.list_para);
        cur_ins->pdata->array_pdata.total_member /= (int)(element);
        left->pdata->array_pdata.step_long =
            cur_ins->pdata->array_pdata.total_member;
        char para_buffer[50];
        memset(para_buffer, 0, sizeof(para_buffer));
        ListFirst(left->pdata->array_pdata.list_para, false);

        while (ListNext(left->pdata->array_pdata.list_para, &element) != NULL) {
          char text[10];
          sprintf(text, "[%d x ", (int)element);
          strcat(para_buffer, text);
        }
        strcat(para_buffer, "i32");
        for (int ii = 0; ii < ListSize(left->pdata->array_pdata.list_para);
             ii++) {
          strcat(para_buffer, "]");
        }
        printf(
            "%s = getelementptr inbounds %s, %s"
            " * %s, i32 0, i32 %s, !dbg !24\n",
            cur_ins->name, para_buffer, para_buffer, left->name, right->name);
        ListPushBack(ins_list, cur_ins);
        if (ListSize(cur_ins->pdata->array_pdata.list_para) == 0 &&
            (pre_astnode->r ? strcmp(pre_astnode->r->name, "ASSIGNOP")
                            : true)) {
          // 链表为空 代表数组退化为普通的指针
          // 如果不是对数组里面的成员赋值则要把内容load出来使用
          char temp_str[15];
          char text[10];
          sprintf(text, "%d", temp_var_seed);
          ++temp_var_seed;
          strcpy(temp_str, "\%temp");
          strcat(temp_str, text);
          // cur_ins->VTy->TID = PointerTyID;
          // 内容与指针所指向的pdata完全一样 名字不一样 占用的内存地址也不一样
          Value *load_ins =
              (Value *)ins_new_single_operator_v2(LoadOP, cur_ins);
          load_ins->name = strdup(temp_str);
          // 将内容拷贝
          value_copy(load_ins, cur_ins->pdata->array_pdata.array_value[0]
                                   .pdata->allocate_pdata.point_value);

          ListPushBack(ins_list, (void *)load_ins);

          printf("%s = load %s, %s,align 4\n", temp_str,
                 NowVarDecStr[load_ins->VTy->TID < 4 ? load_ins->VTy->TID
                                                     : load_ins->VTy->TID - 4],
                 cur_ins->name);

          return load_ins;
        }
        return cur_ins;
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

        char *oprand_type =
            NowVarDecStr[cur_ins->VTy->TID < 4 ? cur_ins->VTy->TID
                                               : cur_ins->VTy->TID - 4];

        if (!strcmp(a->r->name, "PLUS")) {
          ((Instruction *)cur_ins)->opcode = AddOP;
          printf("%s = add nsw %s %s, %s\n", cur_ins->name, oprand_type,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "MINUS")) {
          ((Instruction *)cur_ins)->opcode = SubOP;
          printf("%s = sub nsw %s %s, %s\n", cur_ins->name, oprand_type,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "STAR")) {
          ((Instruction *)cur_ins)->opcode = MulOP;
          printf("%s = mul nsw %s %s, %s\n", cur_ins->name, oprand_type,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "DIV")) {
          ((Instruction *)cur_ins)->opcode = DivOP;
          printf("%s = div nsw %s %s, %s\n", cur_ins->name, oprand_type,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "EQUAL")) {
          ((Instruction *)cur_ins)->opcode = EqualOP;
          printf("%s = icmp eq %s %s, %s\n", cur_ins->name, oprand_type,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "NOTEQUAL")) {
          ((Instruction *)cur_ins)->opcode = NotEqualOP;
          printf("%s = icmp neq %s %s, %s\n", cur_ins->name, oprand_type,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "GREAT")) {
          ((Instruction *)cur_ins)->opcode = GreatThanOP;
          printf("%s = icmp > %s %s, %s\n", cur_ins->name, oprand_type,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "LESS")) {
          ((Instruction *)cur_ins)->opcode = LessThanOP;
          printf("%s = icmp < %s %s, %s\n", cur_ins->name, oprand_type,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "GREATEQUAL")) {
          ((Instruction *)cur_ins)->opcode = GreatEqualOP;
          printf("%s = icmp >= %s %s, %s\n", cur_ins->name, oprand_type,
                 left->name, right->name);
        } else if (!strcmp(a->r->name, "LESSEQUAL")) {
          ((Instruction *)cur_ins)->opcode = LessEqualOP;
          printf("%s = icmp <= %s %s, %s\n", cur_ins->name, oprand_type,
                 left->name, right->name);
        }
        ListPushBack(ins_list, (void *)cur_ins);
        return cur_ins;
      }
      return left;
    }

    if (!strcmp(a->name, "assistFuncCall")) {
      // param_seed = 1;
      return right;
    }

    // 后续遍历到if标识该if管辖的全区域块结束 插入跳转点label
    if (!strcmp(a->name, "IF")) {
      Instruction *ins_back = NULL;
      ListGetBack(ins_list, (void **)&ins_back);

      // 当前的if不含else的情况
      if (!have_else) {
        // 删除链尾 并且释放链尾ins的内存
        // ListPopBack(ins_list);
        // value_free((Value *)ins_back);
        // free(ins_back);

        Value *else_label_ins = NULL;
        StackTop(stack_else_label, (void **)&else_label_ins);
        StackPop(stack_else_label);
        // 重定向无条件跳转的指向
        char temp_str[30];
        strcpy(temp_str, "goto ");
        strcat(temp_str, ((Value *)else_label_ins)->name);
        free(((Value *)ins_back)->name);
        ((Value *)ins_back)->name = strdup(temp_str);
        ((Value *)ins_back)->pdata->no_condition_goto.goto_location =
            else_label_ins;
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
        have_else = false;
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
      // // pdata不需要数据所以释放掉
      // free(func_end_ins->pdata);

      // 插入
      ListPushBack(ins_list, (void *)func_end_ins);

      printf("%s\n", temp_str);
    }

    if (!strcmp(a->name, "RETURN")) {
      char temp_str[20];

      Value *func_return_ins =
          (Value *)ins_new_single_operator_v2(ReturnOP, right);
      // 添加变量的名字 类型 和返回值
      func_return_ins->name = strdup("return");
      func_return_ins->VTy->TID = ReturnTyID;

      // 插入
      ListPushBack(ins_list, (void *)func_return_ins);

      printf("%s\n", func_return_ins->name);
    }

    if (!strcmp(a->name, "ELSE")) {
      have_else = true;
      Value *then_label_ins = NULL;
      StackTop(stack_then_label, (void **)&then_label_ins);
      char temp_str[30];
      strcpy(temp_str, "goto ");
      strcat(temp_str, then_label_ins->name);

      Value *goto_then_ins = (Value *)ins_new_no_operator_v2(GotoOP);
      goto_then_ins->name = strdup(temp_str);
      goto_then_ins->VTy->TID = GotoTyID;
      goto_then_ins->pdata->no_condition_goto.goto_location = then_label_ins;

      ListPushBack(ins_list, (void *)goto_then_ins);
    }

    if (!strcmp(a->name, "WHILE")) {
      Value *while_head_label_ins = NULL;
      StackTop(stack_while_head_label, (void **)&while_head_label_ins);
      StackPop(stack_while_head_label);

      char temp_str[30];
      strcpy(temp_str, "goto ");
      strcat(temp_str, while_head_label_ins->name);

      // 跳出while循环
      Value *goto_out_while_ins = (Value *)ins_new_no_operator_v2(GotoOP);
      goto_out_while_ins->name = strdup(temp_str);
      goto_out_while_ins->VTy->TID = GotoTyID;
      goto_out_while_ins->pdata->no_condition_goto.goto_location =
          while_head_label_ins;

      ListPushBack(ins_list, (void *)goto_out_while_ins);

      // 跳出while循环后紧接着语句的label
      Value *while_then_label_ins = NULL;
      StackTop(stack_while_then_label, (void **)&while_then_label_ins);
      StackPop(stack_while_then_label);
      ListPushBack(ins_list, (void *)while_then_label_ins);

      printf("%s\n", while_then_label_ins->name);
      return NULL;
    }

    if (!strcmp(a->name, "BREAK")) {
      Value *goto_break_label_ins = NULL;
      if (StackSize(stack_while_then_label) == 0) {
        // 没有地方可以去 报错？
        return NULL;
      }

      StackTop(stack_while_then_label, (void **)&goto_break_label_ins);

      char temp_str[30];
      strcpy(temp_str, "(break)br :");
      strcat(temp_str, goto_break_label_ins->name);

      Value *break_ins = (Value *)ins_new_no_operator_v2(GotoOP);
      // 添加变量的名字 类型 和返回值
      break_ins->name = strdup(temp_str);
      break_ins->VTy->TID = GotoTyID;
      break_ins->pdata->no_condition_goto.goto_location = goto_break_label_ins;

      ListPushBack(ins_list, (void *)break_ins);
      printf("break :br %s \n", goto_break_label_ins->name);
    }

    if (!strcmp(a->name, "CONTINUE")) {
      Value *goto_head_label_ins = NULL;
      if (StackSize(stack_while_head_label) == 0) {
        // 没有地方可以去 报错？
        return NULL;
      }

      StackTop(stack_while_head_label, (void **)&goto_head_label_ins);

      char temp_str[30];
      strcpy(temp_str, "(continue)br :");
      strcat(temp_str, goto_head_label_ins->name);

      Value *break_ins = (Value *)ins_new_no_operator_v2(GotoOP);
      // 添加变量的名字 类型 和返回值
      break_ins->name = strdup(temp_str);
      break_ins->VTy->TID = GotoTyID;
      break_ins->pdata->no_condition_goto.goto_location = goto_head_label_ins;

      ListPushBack(ins_list, (void *)break_ins);
      printf("continue :br %s \n", goto_head_label_ins->name);
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
