#include "Pass.h"

char *op_string[] = {"DefaultOP",
                     "AddOP",
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
                     "FuncEndOP",
                     "AllocateOP",
                     "LoadOP",
                     "StoreOP"};

extern BasicBlock *cur_bblock;

extern Function *cur_func;

extern List *func_list;

extern HashMap *bblock_hashmap;

extern HashSet *bblock_pass_hashset;

extern HashMap *bblock_to_dom_graph_hashmap;

ALGraph *graph_for_dom_tree = NULL;

HashSet *graph_head_set = NULL;

TAC_OP pre_op;

BasicBlock *name_get_bblock(char *name) {
  return (BasicBlock *)HashMapGet(bblock_hashmap, name);
}

void dom_tree_pass(List *self) {
  int i = 0;
  // printf("\nbegin print the instruction: \n");
  void *element;
  ListFirst(self, false);
  while (ListNext(self, &element)) {
    // 打印出每条instruction的res的名字信息
    printf("%9s\t     %d: %20s ", "", i++,
           op_string[((Instruction *)element)->opcode]);

    printf("\t%25s ", ((Value *)element)->name);

    if (((Instruction *)element)->user.num_oprands == 2) {
      printf("\t%10s, %10s\n",
             user_get_operand_use(((User *)element), 0)->Val->name,
             user_get_operand_use(((User *)element), 1)->Val->name);
    } else if (((Instruction *)element)->user.num_oprands == 1) {
      printf("\t%10s\n", user_get_operand_use(((User *)element), 0)->Val->name);
    } else if (((Instruction *)element)->user.num_oprands == 0) {
      printf("\t%10s\n", "null");
    }
  }
}

// 判断self是否为other的子集
bool is_subset(HashSet *self, HashSet *other) {
  // 求交集
  HashSet *inter = HashSetIntersect(self, other);
  bool is_self_subset_to_other = (HashSetSize(inter) == HashSetSize(self));
  HashSetDeinit(inter);
  return is_self_subset_to_other;
}

// bblock关系图转化为CFG邻接表图形式
int bblock_to_dom_graph_dfs_pass(HeadNode *self, int n) {
  // if (self != NULL &&
  //     HashSetFind(bblock_pass_hashset, self->bblock_head) == false) {
  graph_for_dom_tree->node_set[n++] = self;

  HashMapPut(bblock_to_dom_graph_hashmap,
             strdup(self->bblock_head->label->name), self);

  printf("%s\n", self->bblock_head->label->name);

  if (self->bblock_head->true_bblock) {
    if (HashMapContain(bblock_to_dom_graph_hashmap,
                       self->bblock_head->true_bblock->label->name)) {
      HeadNode *true_situation_headnode =
          HashMapGet(bblock_to_dom_graph_hashmap,
                     self->bblock_head->true_bblock->label->name);

      HashSetAdd(self->edge_list, true_situation_headnode);
      ListPushBack(true_situation_headnode->pre_node_list, self);
    } else {
      HeadNode *true_situation_headnode = (HeadNode *)malloc(sizeof(HeadNode));
      true_situation_headnode->bblock_head = self->bblock_head->true_bblock;
      true_situation_headnode->is_visited = false;

      // 初始化哈希集 并且将true条件下跳转的的bblock加入当前表头的hash集中
      // 需要更改指针的指向所以需要传递指针的地址
      hashset_init(&(true_situation_headnode->edge_list));
      hashset_init(&(true_situation_headnode->dom_set));
      hashset_init(&(true_situation_headnode->dom_frontier_set));

      // 初始化前驱链表
      true_situation_headnode->pre_node_list = ListInit();
      ListSetClean(true_situation_headnode->pre_node_list, CleanObject);
      ListPushBack(true_situation_headnode->pre_node_list, self);

      HashSetAdd(self->edge_list, true_situation_headnode);
      n = bblock_to_dom_graph_dfs_pass(true_situation_headnode, n);
    }
  }
  if (self->bblock_head->false_bblock) {
    if (HashMapContain(bblock_to_dom_graph_hashmap,
                       self->bblock_head->false_bblock->label->name)) {
      HeadNode *false_situation_headnode =
          HashMapGet(bblock_to_dom_graph_hashmap,
                     self->bblock_head->false_bblock->label->name);

      HashSetAdd(self->edge_list, false_situation_headnode);
      ListPushBack(false_situation_headnode->pre_node_list, self);
    } else {
      HeadNode *false_situation_headnode = (HeadNode *)malloc(sizeof(HeadNode));
      false_situation_headnode->bblock_head = self->bblock_head->false_bblock;
      false_situation_headnode->is_visited = false;
      hashset_init(&(false_situation_headnode->edge_list));
      hashset_init(&(false_situation_headnode->dom_set));
      hashset_init(&(false_situation_headnode->dom_frontier_set));
      // 初始化前驱链表
      false_situation_headnode->pre_node_list = ListInit();
      ListSetClean(false_situation_headnode->pre_node_list, CleanObject);
      ListPushBack(false_situation_headnode->pre_node_list, self);

      HashSetAdd(self->edge_list, false_situation_headnode);
      n = bblock_to_dom_graph_dfs_pass(false_situation_headnode, n);
    }
  }

  return n;
}

void dom_relation_pass_help(HeadNode *self) {
  self->is_visited = true;
  void *element;
  HashSetFirst(self->edge_list);
  while ((element = HashSetNext(self->edge_list)) &&
         !((HeadNode *)element)->is_visited) {
    // printf("cur node %s next node %s\n", self->bblock_head->label->name,
    //        ((HeadNode *)element)->bblock_head->label->name);
    dom_relation_pass_help((HeadNode *)element);
  }
}

void dom_relation_pass() {
  int node_num = graph_for_dom_tree->node_num;
  printf("node entry dom ");
  for (int j = 0; j < node_num; j++) {
    printf("%s,", graph_for_dom_tree->node_set[j]->bblock_head->label->name);
  }
  printf("\n");

  for (int i = 1; i < node_num; i++) {
    // 删除该节点的入边和出边 计算出根节点的不可达节点便是该节点的支配节点
    int delete_marked[node_num];
    // 数组内容全部初始化为0
    memset(delete_marked, 0, node_num * sizeof(int));

    for (int j = 0; j < node_num; j++) {
      if (j == i) {
        // 自己需不需要支配自己 不需要就将自己的visited置为true
        continue;
      }
      if (HashSetFind(graph_for_dom_tree->node_set[j]->edge_list,
                      graph_for_dom_tree->node_set[i])) {
        // 谁remove了谁就要添加回来
        delete_marked[j] = 1;
        HashSetRemove(graph_for_dom_tree->node_set[j]->edge_list,
                      graph_for_dom_tree->node_set[i]);
      }
    }

    dom_relation_pass_help(graph_for_dom_tree->node_set[0]);

    for (int j = 0; j < node_num; j++) {
      if (delete_marked[j] == 1) {
        HashSetAdd(graph_for_dom_tree->node_set[j]->edge_list,
                   graph_for_dom_tree->node_set[i]);
        // printf("fix %s to %s\n",
        //        graph_for_dom_tree->node_set[j]->bblock_head->label->name,
        //        graph_for_dom_tree->node_set[i]->bblock_head->label->name);
      }
    }

    printf("node %s dom ",
           graph_for_dom_tree->node_set[i]->bblock_head->label->name);
    for (int j = 0; j < node_num; j++) {
      if (!graph_for_dom_tree->node_set[j]->is_visited) {
        // printf("%s\n",graph_for_dom_tree->node_set[j]->bblock_head->label->name);
        // 添加支配节点
        HashSetAdd(graph_for_dom_tree->node_set[i]->dom_set,
                   graph_for_dom_tree->node_set[j]);
        printf("%s,",
               graph_for_dom_tree->node_set[j]->bblock_head->label->name);
      } else {
        graph_for_dom_tree->node_set[j]->is_visited = false;
      }
    }
    printf("\n");
  }

  printf("\n");
  // 计算每个节点的idom
  for (int i = 1; i < node_num; i++) {
    int cur_idom_nodeset_num = INT_MAX;
    int cur_subscript = 0;

    for (int j = 1; j < node_num; j++) {
      if (i == j) {
        // 忽略自己 自己的idom不能是自己
        continue;
      }
      if (is_subset(graph_for_dom_tree->node_set[i]->dom_set,
                    graph_for_dom_tree->node_set[j]->dom_set) &&
          (HashSetSize(graph_for_dom_tree->node_set[j]->edge_list) <
           cur_idom_nodeset_num)) {
        cur_subscript = j;
        cur_idom_nodeset_num =
            HashSetSize(graph_for_dom_tree->node_set[j]->edge_list);
      }
    }

    graph_for_dom_tree->node_set[i]->idom_node =
        graph_for_dom_tree->node_set[cur_subscript];
    printf(
        "idom(%s) = %s\n",
        graph_for_dom_tree->node_set[i]->bblock_head->label->name,
        graph_for_dom_tree->node_set[i]->idom_node->bblock_head->label->name);
  }

  printf("\n");

  // 每个节点的前驱节点
  for (int i = 1; i < node_num; i++) {
    // printf("cur node %s's pre node is ",
    //        graph_for_dom_tree->node_set[i]->bblock_head->label->name);
    // HeadNode *element;
    // ListFirst(graph_for_dom_tree->node_set[i]->pre_node_list, false);
    // while (ListNext(graph_for_dom_tree->node_set[i]->pre_node_list,
    // &element)) {
    //   printf("%s,", element->bblock_head->label->name);
    // }
    // printf("\n");
    if (ListSize(graph_for_dom_tree->node_set[i]->pre_node_list) > 1) {
      HeadNode *runner;
      ListFirst(graph_for_dom_tree->node_set[i]->pre_node_list, false);
      while (
          ListNext(graph_for_dom_tree->node_set[i]->pre_node_list, &runner)) {
        while (runner != graph_for_dom_tree->node_set[i]->idom_node) {
          // printf("%s,", runner->bblock_head->label->name);
          HashSetAdd(runner->dom_frontier_set, graph_for_dom_tree->node_set[i]);
          runner = runner->idom_node;
        }
        // printf("\n");
      }
      // printf("\n");
    }
  }

  for (int i = 1; i < node_num; i++) {
    printf("cur node %s's dom frontier is ",
           graph_for_dom_tree->node_set[i]->bblock_head->label->name);

    HeadNode *element;
    HashSetFirst(graph_for_dom_tree->node_set[i]->dom_frontier_set);
    while ((element = HashSetNext(
                graph_for_dom_tree->node_set[i]->dom_frontier_set)) != NULL) {
      printf("%s,", element->bblock_head->label->name);
    }
    printf("\n");
  }
}

void insert_phi_func() {}

// 生成支配树的pass
void bblock_to_dom_graph_pass(Function *self) {
  int num_of_block = self->num_of_block;
  // 设置支配树对应图的邻接表头
  hashset_init(&(graph_head_set));
  graph_for_dom_tree = (ALGraph *)malloc(sizeof(ALGraph));
  graph_for_dom_tree->node_set =
      (HeadNode **)malloc(num_of_block * sizeof(HeadNode));
  graph_for_dom_tree->node_num = num_of_block;

  // 设置CFG图的入口基本块表头 并且初始化链表
  HeadNode *init_headnode = (HeadNode *)malloc(sizeof(HeadNode));
  init_headnode->bblock_head = self->entry_bblock;
  init_headnode->is_visited = false;
  hashset_init(&(init_headnode->edge_list));
  hashset_init(&(init_headnode->dom_set));
  hashset_init(&(init_headnode->dom_frontier_set));
  init_headnode->pre_node_list = NULL;

  bblock_to_dom_graph_dfs_pass(init_headnode, 0);
  HashMapDeinit(bblock_to_dom_graph_hashmap);
  printf("\n");

  dom_relation_pass();
}

// ins_list
void print_ins_pass(List *self) {
  int i = 0;
  // printf("\nbegin print the instruction: \n");
  void *element;
  ListFirst(self, false);
  while (ListNext(self, &element)) {
    // 打印出每条instruction的res的名字信息
    printf("%9s\t     %d: %20s ", "", i++,
           op_string[((Instruction *)element)->opcode]);

    printf("\t%25s ", ((Value *)element)->name);
    if (((Instruction *)element)->user.num_oprands == 2) {
      printf("\t%10s, %10s\n",
             user_get_operand_use(((User *)element), 0)->Val->name,
             user_get_operand_use(((User *)element), 1)->Val->name);
    } else if (((Instruction *)element)->user.num_oprands == 1) {
      printf("\t%10s\n", user_get_operand_use(((User *)element), 0)->Val->name);
    } else if (((Instruction *)element)->user.num_oprands == 0) {
      printf("\t%10s\n", "null");
    }
  }
}

void print_bblock_pass(BasicBlock *self) {
  if (self != NULL && HashSetFind(bblock_pass_hashset, self) == false) {
    printf("\t%s:\n", self->label->name);
    HashSetAdd(bblock_pass_hashset, self);
    print_ins_pass(self->inst_list);
    printf("\n");
    print_bblock_pass(self->true_bblock);
    print_bblock_pass(self->false_bblock);
  }
}

void ins_toBBlock_pass(List *self) {
  // 顺序遍历
  void *element;
  ListFirst(self, false);
  // 迭代下标
  int iterate_subscript = -1;

  while (ListNext(self, &element)) {
    iterate_subscript++;
    // 初始包含入口基本块和结束基本块
    int num_of_block = 2;
    //  进入一个函数
    if (((Instruction *)element)->opcode == FuncLabelOP) {
      // 初始化函数 将函数里的label等同于Funclabel中的value*
      cur_func = (Function *)malloc(sizeof(Function));
      function_init(cur_func);
      cur_func->label = (Value *)element;
      // 初始化entryLabel并且插入到函数的入口label
      ListNext(self, &element);
      iterate_subscript++;
      cur_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
      bblock_init(cur_bblock, cur_func);
      cur_bblock->label = (Value *)element;

      // 设置当前的函数的入口基本块
      cur_func->entry_bblock = cur_bblock;
      ListPushBack(cur_bblock->inst_list, element);
      // 插入函数链表中
      ListPushBack(func_list, cur_func);

      // 创建end_block和对应的end_label用于解决return语句唯一出口的问题
      BasicBlock *end_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
      bblock_init(end_bblock, cur_func);

      // 创建end条件下的label标签
      Value *end_label_ins = (Value *)ins_new_no_operator_v2(LabelOP);
      // 添加变量的名字
      end_label_ins->name = strdup("end_label");
      end_label_ins->VTy->TID = LabelTyID;
      end_bblock->label = end_label_ins;
      ListPushBack(end_bblock->inst_list, end_label_ins);
      // 设置当前函数的结束基本块
      cur_func->end_bblock = end_bblock;
      // printf("successfully initialization functionblock\n");

      // 插入instruction
      while (ListNext(self, &element)) {
        iterate_subscript++;
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
            // 将当前bblock置为后续bblock的父基本块
            ListPushBack(cur_bblock->true_bblock->father_bblock_list,
                         cur_bblock);
            ListPushBack(cur_bblock->false_bblock->father_bblock_list,
                         cur_bblock);
            cur_bblock->true_bblock->label =
                ((Value *)element)->pdata->condition_goto.true_goto_location;
            HashMapPut(bblock_hashmap,
                       strdup(cur_bblock->true_bblock->label->name),
                       cur_bblock->true_bblock);
            cur_bblock->false_bblock->label =
                ((Value *)element)->pdata->condition_goto.false_goto_location;
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
                ((Value *)element)->pdata->no_condition_goto.goto_location;
            HashMapPut(bblock_hashmap,
                       strdup(cur_bblock->true_bblock->label->name),
                       cur_bblock->true_bblock);
            break;

          case LabelOP:
            num_of_block++;
            // printf(" %s ins is printed\n",
            //        op_string[((Instruction *)element)->opcode]);
            if (pre_op != GotoOP && pre_op != ReturnOP &&
                pre_op != GotoWithConditionOP) {
              // printf("%s is cur label %s is next label \n",
              //        cur_bblock->label->name, ((User
              //        *)element)->res->name);
              cur_bblock->true_bblock =
                  name_get_bblock(((Value *)element)->name);
              ListPushBack(
                  name_get_bblock(((Value *)element)->name)->father_bblock_list,
                  cur_bblock);
            }
            cur_bblock = name_get_bblock(((Value *)element)->name);

            ListPushBack(cur_bblock->inst_list, element);
            break;

          case FuncEndOP:
            if (pre_op != ReturnOP) {
              cur_bblock->true_bblock = end_bblock;
            }
            ListPushBack(end_bblock->inst_list, element);
            break;

          case ReturnOP:
            cur_bblock->true_bblock = end_bblock;
            ListPushBack(cur_bblock->inst_list, element);
            break;
          // case AllocateOP:
          //   ListRemove()
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
    cur_func->num_of_block = num_of_block;

    // // 打印当前函数的基本块
    // print_bblock_pass(cur_func->entry_bblock);
    // // 清空哈希表 然后重新初始化供后面使用
    // HashSetDeinit(bblock_pass_hashset);
    // hashset_init(&(bblock_pass_hashset));
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
