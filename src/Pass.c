#include "Pass.h"

char *op_string[] = {"AddOP",
                     "SubOP",
                     "MulOP",
                     "DivOP",
                     "GotoOP",
                     "GotoWithConditionOP",
                     "CallOP",
                     "FunBeginOP",
                     "FunEndOP",
                     "ReturnOP",
                     "ParamOP",
                     "AssignOP",
                     "CallWithReturnValueOP",
                     "NotEqualOP",
                     "EqualOP",
                     "GreatThanOP",
                     "LessThanOP",
                     "GreatEqualOP",
                     "LessEqualOP",
                     "LabelOP",
                     "FuncLabelOP",
                     "FuncEndOP"};

extern BasicBlock *cur_bblock;

void print_ins_pass(List *self) {
  printf("\nbegin print the instruction: \n");
  void *element;
  ListFirst(self, false);
  while (ListNext(self, &element)) {
    // 打印出每条instruction的res的名字信息
    printf("opcode: %s    ", op_string[((Instruction *)element)->opcode]);
    printf("\tname: %s    ", ((Instruction *)element)->user.res->name);
    if (((Instruction *)element)->user.res->NumUserOperands == 2) {
      printf("\t%s\t%s\n",
             user_get_operand_use(((User *)element), 0)->Val->name,
             user_get_operand_use(((User *)element), 1)->Val->name);
    } else if (((Instruction *)element)->user.res->NumUserOperands == 1) {
      printf("\t%s\n", user_get_operand_use(((User *)element), 0)->Val->name);
    } else if (((Instruction *)element)->user.res->NumUserOperands == 0) {
      printf("\t0\n");
    }
  }
}

void ins_toBBlock_pass(List *self) {
  void *element;
  // 顺序遍历
  ListFirst(self, false);

  while (ListNext(self, &element)) {
    // 进入一个函数
    if (((Instruction *)element)->opcode == FuncLabelOP) {
      // 插入entryLabel;
      ListNext(self, &element);
      cur_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
      bblock_init(cur_bblock);
      cur_bblock->label = ((User *)element)->res;
      // 插入instruction
      while (ListNext(self, &element)) {
        if (((Instruction *)element)->opcode == GotoWithConditionOP) {
          ListPushBack(cur_bblock->inst_list, element);
          // 初始化要跳转的两个基本块
          cur_bblock->true_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
          cur_bblock->false_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
          bblock_init(cur_bblock->true_bblock);
          bblock_init(cur_bblock->false_bblock);
          cur_bblock->true_bblock->label =
              ((User *)element)->res->pdata->condition_goto.true_goto_location;
          cur_bblock->false_bblock->label =
              ((User *)element)->res->pdata->condition_goto.false_goto_location;
        }
      }
    }
  }
}