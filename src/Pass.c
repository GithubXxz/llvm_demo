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

extern Function *cur_func;

extern List *func_list;

extern HashMap *bblock_hashmap;

extern HashSet *bblock_pass_hashset;

TAC_OP pre_op;

BasicBlock *name_get_bblock(char *name) {
  return (BasicBlock *)HashMapGet(bblock_hashmap, name);
}

void print_ins_pass(List *self) {
  int i = 0;
  // printf("\nbegin print the instruction: \n");
  void *element;
  ListFirst(self, false);
  while (ListNext(self, &element)) {
    // 打印出每条instruction的res的名字信息
    printf("%d: opcode: %s    ", i++,
           op_string[((Instruction *)element)->opcode]);
    printf("\tname: %s    ", ((Instruction *)element)->user.res->name);
    if (((Instruction *)element)->user.num_oprands == 2) {
      printf("\t%s\t%s\n",
             user_get_operand_use(((User *)element), 0)->Val->name,
             user_get_operand_use(((User *)element), 1)->Val->name);
    } else if (((Instruction *)element)->user.num_oprands == 1) {
      printf("\t%s\n", user_get_operand_use(((User *)element), 0)->Val->name);
    } else if (((Instruction *)element)->user.num_oprands == 0) {
      printf("\t0\n");
    }
  }
}

void print_bblock_pass(BasicBlock *self) {
  if (self != NULL && HashSetFind(bblock_pass_hashset, self) == false) {
    HashSetAdd(bblock_pass_hashset, self);
    print_ins_pass(self->inst_list);
    printf("\n");
    print_bblock_pass(self->true_bblock);
    print_bblock_pass(self->false_bblock);
  }
}

void ins_toBBlock_pass(List *self) {
  void *element;
  // 顺序遍历
  ListFirst(self, false);

  while (ListNext(self, &element)) {
    // 进入一个函数
    if (((Instruction *)element)->opcode == FuncLabelOP) {
      // 初始化函数 将函数里的label等同于Funclabel中的value*
      cur_func = (Function *)malloc(sizeof(Function));
      function_init(cur_func);
      cur_func->label = ((User *)element)->res;
      // 初始化entryLabel并且插入到函数的入口label
      ListNext(self, &element);
      cur_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
      bblock_init(cur_bblock, cur_func);
      cur_bblock->label = ((User *)element)->res;

      // 设置当前的函数的入口基本块
      cur_func->entry_bblock = cur_bblock;
      ListPushBack(cur_bblock->inst_list, element);
      // 插入函数链表中
      ListPushBack(func_list, cur_func);

      // 创建end_block和对应的end_label用于解决return语句唯一出口的问题
      BasicBlock *end_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
      bblock_init(end_bblock, cur_func);

      // 创建end条件下的label标签
      Value *end_label = (Value *)malloc(sizeof(Value));
      value_init(end_label);
      // 添加变量的名字
      end_label->name = strdup("end_label");
      end_label->VTy->TID = LabelTyID;
      Instruction *end_label_ins = ins_new_no_operator(end_label, LabelOP);
      end_bblock->label = end_label;
      cur_func->end_bblock = end_bblock;
      // printf("successfully initialization functionblock\n");

      // 插入instruction
      while (ListNext(self, &element)) {
        TAC_OP cur_ins_opcode = ((Instruction *)element)->opcode;
        switch (cur_ins_opcode) {
          case GotoWithConditionOP:
            // printf("%s label %s ins is printed\n", cur_bblock->label->name,
            //        op_string[((Instruction *)element)->opcode]);
            ListPushBack(cur_bblock->inst_list, element);
            // 初始化要跳转的两个基本块
            cur_bblock->true_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
            cur_bblock->false_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
            bblock_init(cur_bblock->true_bblock, cur_func);
            bblock_init(cur_bblock->false_bblock, cur_func);
            ListPushBack(cur_bblock->true_bblock->father_bblock_list,
                         cur_bblock);
            ListPushBack(cur_bblock->false_bblock->father_bblock_list,
                         cur_bblock);
            cur_bblock->true_bblock->label =
                ((User *)element)
                    ->res->pdata->condition_goto.true_goto_location;
            HashMapPut(bblock_hashmap,
                       strdup(cur_bblock->true_bblock->label->name),
                       cur_bblock->true_bblock);
            cur_bblock->false_bblock->label =
                ((User *)element)
                    ->res->pdata->condition_goto.false_goto_location;
            HashMapPut(bblock_hashmap,
                       strdup(cur_bblock->false_bblock->label->name),
                       cur_bblock->false_bblock);
            break;

          case GotoOP:
            // printf("%s label %s ins is printed\n", cur_bblock->label->name,
            //        op_string[((Instruction *)element)->opcode]);
            ListPushBack(cur_bblock->inst_list, element);
            // 初始化要跳转的一个基本块
            cur_bblock->true_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
            bblock_init(cur_bblock->true_bblock, cur_func);
            ListPushBack(cur_bblock->true_bblock->father_bblock_list,
                         cur_bblock);
            cur_bblock->true_bblock->label =
                ((User *)element)->res->pdata->no_condition_goto.goto_location;
            HashMapPut(bblock_hashmap,
                       strdup(cur_bblock->true_bblock->label->name),
                       cur_bblock->true_bblock);
            break;

          case LabelOP:
            // printf(" %s ins is printed\n",
            //        op_string[((Instruction *)element)->opcode]);
            if (pre_op != GotoOP && pre_op != ReturnOP &&
                pre_op != GotoWithConditionOP) {
              // printf("%s is cur label %s is next label \n",
              //        cur_bblock->label->name, ((User *)element)->res->name);
              cur_bblock->true_bblock =
                  name_get_bblock(((User *)element)->res->name);
              ListPushBack(name_get_bblock(((User *)element)->res->name)
                               ->father_bblock_list,
                           cur_bblock);
            }
            cur_bblock = name_get_bblock(((User *)element)->res->name);

            ListPushBack(cur_bblock->inst_list, element);
            break;

          case FuncEndOP:
            if (pre_op != ReturnOP) {
              cur_bblock->true_bblock = end_bblock;
            }
            ListPushBack(end_bblock->inst_list, end_label_ins);
            ListPushBack(end_bblock->inst_list, element);
            break;

          case ReturnOP:
            cur_bblock->true_bblock = end_bblock;
            ListPushBack(cur_bblock->inst_list, element);
            break;

          default:
            // printf("%s label %s ins push back\n", cur_bblock->label->name,
            //        op_string[((Instruction *)element)->opcode]);
            ListPushBack(cur_bblock->inst_list, element);
            break;
        }

        pre_op = ((Instruction *)element)->opcode;
        if (cur_ins_opcode == FuncEndOP) {
          break;
        }
      }
    }
  }
}

void delete_return_deadcode_pass(List *self) {
  void *element;
  ListFirst(self, false);
  ListSetClean(self, CommonCleanInstruction);
  unsigned i = 0;
  while (ListNext(self, &element)) {
    if (((Instruction *)element)->opcode == ReturnOP) {
      i++;
      while (ListNext(self, &element) &&
             (((Instruction *)element)->opcode != LabelOP &&
              ((Instruction *)element)->opcode != FuncEndOP)) {
        ListRemove(self, i);
      }
    }
    i++;
  }
  ListSetClean(self, CleanObject);
}