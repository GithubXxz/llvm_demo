#include "Pass.h"

#include <stdarg.h>  //变长参数函数所需的头文件
#include <stdio.h>

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
                     "StoreOP",
                     "PhiFuncOp"};

extern BasicBlock *cur_bblock;

extern Function *cur_func;

extern List *func_list;

extern HashMap *bblock_hashmap;

extern HashSet *bblock_pass_hashset;

extern HashMap *bblock_to_dom_graph_hashmap;

ALGraph *graph_for_dom_tree = NULL;

dom_tree *dom_tree_root = NULL;

TAC_OP pre_op;

int phi_var_seed = 1;  // 用于phi函数产生变量的名字

// 通过bblock的名字获得bblock的指针
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

  //// 打印图中的每个节点的后继的名字和地址信息
  // for (int i = 0; i < node_num; i++) {
  //   printf("%s: ",
  //   graph_for_dom_tree->node_set[i]->bblock_head->label->name); void
  //   *element; HashSetFirst(graph_for_dom_tree->node_set[i]->edge_list); while
  //   ((element = HashSetNext(
  //               graph_for_dom_tree->node_set[i]->edge_list)) != NULL) {
  //     printf("%s|%8x,", ((HeadNode *)element)->bblock_head->label->name,
  //            element);
  //   }
  //   printf("\n");
  // }

  printf("\n");

  //// 打印当前函数含有的bblock的数量
  // printf("cur graph has %d node\n", node_num);

  // 打印入口节点的支配节点信息和地址
  printf("node entry dom ");
  for (int j = 0; j < node_num; j++) {
    printf("%s|%p,", graph_for_dom_tree->node_set[j]->bblock_head->label->name,
           graph_for_dom_tree->node_set[j]);
  }
  printf("\n\n");

  for (int i = 1; i < node_num; i++) {
    // 删除该节点的入边和出边 计算出根节点的不可达节点便是该节点的支配节点
    int delete_marked[node_num];
    // 数组内容全部初始化为0
    memset(delete_marked, 0, node_num * sizeof(int));

    //// 打印删除的边的辅助信息
    // printf("delete edge ");
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
        // //打印删除的边
        // printf("%s|%8x,",
        //        graph_for_dom_tree->node_set[j]->bblock_head->label->name,
        //        graph_for_dom_tree->node_set[j]);
      }
    }

    //// 打印删除的边的辅助信息
    //  printf(" to %s|%8x\n",
    //         graph_for_dom_tree->node_set[i]->bblock_head->label->name,
    //         graph_for_dom_tree->node_set[i]);

    dom_relation_pass_help(graph_for_dom_tree->node_set[0]);

    for (int j = 0; j < node_num; j++) {
      if (delete_marked[j] == 1) {
        HashSetAdd(graph_for_dom_tree->node_set[j]->edge_list,
                   graph_for_dom_tree->node_set[i]);
        ////打印修复的边
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

  Stack *dom_tree_stack = NULL;
  dom_tree_stack = StackInit();
  StackSetClean(dom_tree_stack, CleanObject);
  StackPush(dom_tree_stack, dom_tree_root);

  while (StackSize(dom_tree_stack) != 0) {
    void *element;
    StackTop(dom_tree_stack, &element);
    StackPop(dom_tree_stack);
    // 打印树中每一层的信息
    printf("%s: ",
           ((dom_tree *)element)->bblock_node->bblock_head->label->name);
    for (int i = 1; i < node_num; i++) {
      if (graph_for_dom_tree->node_set[i]->idom_node ==
          ((dom_tree *)element)->bblock_node) {
        dom_tree *cur = (dom_tree *)malloc(sizeof(dom_tree));
        cur->bblock_node = graph_for_dom_tree->node_set[i];
        printf("%s,", cur->bblock_node->bblock_head->label->name);
        cur->child = ListInit();
        ListSetClean(cur->child, CleanObject);
        ListPushBack(((dom_tree *)element)->child, cur);
        StackPush(dom_tree_stack, cur);
      }
    }
    printf("\n");
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

    // 寻找支配边界
    if (ListSize(graph_for_dom_tree->node_set[i]->pre_node_list) > 1) {
      void *runner;
      ListFirst(graph_for_dom_tree->node_set[i]->pre_node_list, false);
      while (
          ListNext(graph_for_dom_tree->node_set[i]->pre_node_list, &runner)) {
        while (runner != graph_for_dom_tree->node_set[i]->idom_node) {
          // printf("%s,", runner->bblock_head->label->name);
          HashSetAdd(((HeadNode *)(runner))->dom_frontier_set,
                     graph_for_dom_tree->node_set[i]);
          runner = ((HeadNode *)(runner))->idom_node;
        }
        // printf("\n");
      }
      // printf("\n");
    }
  }

  // 打印每个节点和对饮的支配边界
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

void find_bblock_store_ins_pass(HeadNode *self, Value *pointer) {
  void *element;
  ListFirst(self->bblock_head->inst_list, false);

  while (ListNext(self->bblock_head->inst_list, &element)) {
    if (((Instruction *)element)->opcode == StoreOP &&
        user_get_operand_use(((User *)element), 0)->Val == pointer) {
      // printf("%s is def in %s\n",
      //        pointer->pdata->allocate_pdata.point_value->name,
      //        self->bblock_head->label->name);
      self->is_visited = true;
      return;
    }
  }
}

// hashset的并集会不会造成重大的内存泄漏？
// 找到要插入phi函数的bblock并且插入空的phi函数
void insert_phi_func_pass(Function *self) {
  int num_of_block = self->num_of_block;
  BasicBlock *entry_bblock = self->entry_bblock;
  void *bblock_ins = NULL;
  ListFirst(entry_bblock->inst_list, false);

  // &&!strcmp(((Value *)bblock_ins)->name, "\%point1")
  while (ListNext(entry_bblock->inst_list, &bblock_ins)) {
    if (((Instruction *)bblock_ins)->opcode == AllocateOP) {
      // 也就是迭代支配边界集合
      HashSet *phi_insert_bblock = NULL;
      hashset_init(&(phi_insert_bblock));

      for (int i = 1; i < num_of_block; i++) {
        find_bblock_store_ins_pass(graph_for_dom_tree->node_set[i],
                                   (Value *)bblock_ins);
      }

      for (int i = 1; i < num_of_block; i++) {
        if (graph_for_dom_tree->node_set[i]->is_visited) {
          graph_for_dom_tree->node_set[i]->is_visited = false;
          phi_insert_bblock =
              HashSetUnion(phi_insert_bblock,
                           graph_for_dom_tree->node_set[i]->dom_frontier_set);
        }
      }

      unsigned phi_insert_bblock_element_num = 0;
      while (phi_insert_bblock_element_num != HashSetSize(phi_insert_bblock)) {
        phi_insert_bblock_element_num = HashSetSize(phi_insert_bblock);

        HashSet *cur_pass_add_phi_insert_bblock = NULL;
        hashset_init(&(cur_pass_add_phi_insert_bblock));

        void *element;
        HashSetFirst(phi_insert_bblock);

        while ((element = HashSetNext(phi_insert_bblock)) != NULL) {
          cur_pass_add_phi_insert_bblock =
              HashSetUnion(cur_pass_add_phi_insert_bblock,
                           ((HeadNode *)element)->dom_frontier_set);
        }
        phi_insert_bblock =
            HashSetUnion(phi_insert_bblock, cur_pass_add_phi_insert_bblock);
      }

      // 根据 phi_insert_bblock 这个集合插入phi函数
      HeadNode *element;
      HashSetFirst(phi_insert_bblock);

      while ((element = HashSetNext(phi_insert_bblock)) != NULL) {
        //// 打印要插入phi函数的基本块的名字信息
        // printf("%s\n", element->bblock_head->label->name);
        // 在内存中为phi函数分配空间
        Value *cur_var = (Value *)malloc(sizeof(Value));
        value_init(cur_var);
        // 添加变量类型
        cur_var->VTy->TID = PhiFuncTyID;
        // 添加变量的名字
        cur_var->name = strdup("phi_func");

        // 初始化phi函数的hashmap
        cur_var->pdata->phi_func_pdata.phi_value = HashMapInit();

        HashMapSetHash(cur_var->pdata->phi_func_pdata.phi_value, HashKey);
        HashMapSetCompare(cur_var->pdata->phi_func_pdata.phi_value, CompareKey);
        HashMapSetCleanKey(cur_var->pdata->phi_func_pdata.phi_value,
                           CleanHashMapKey);
        HashMapSetCleanValue(cur_var->pdata->phi_func_pdata.phi_value,
                             CleanValue);

        // 给phi函数返回的 Value* 变量命名
        char temp_str[15];
        char text[10];
        sprintf(text, "%d", phi_var_seed);
        ++phi_var_seed;
        strcpy(temp_str, "\%phi_var");
        strcat(temp_str, text);

        // 创建phi函数语句 左值是被定义的变量 可以被引用
        // 第一个操作数是phi函数 第二个操作数是phi函数所对应的变量的指针
        Value *cur_ins = (Value *)ins_new_binary_operator_v2(
            PhiFuncOp, cur_var, (Value *)bblock_ins);
        // 添加变量类型
        cur_ins->VTy->TID =
            ((Value *)bblock_ins)->pdata->allocate_pdata.point_value->VTy->TID;
        // 添加变量的名字
        cur_ins->name = strdup(temp_str);

        // printf("%s = phi,align 4\n", temp_str);

        ListInsert(element->bblock_head->inst_list, 1, cur_ins);
      }
    }
  }
}

// 将所有用到other的地方全部用self替换
void replace_use_other_by_self(Value *self, Value *other) {
  if (other->use_list != NULL) {
    Use *u1 = other->use_list;
    Use *u2 = u1->Next;
    while (u1 != NULL) {
      value_add_use(self, u1);
      u1 = u2;
      u2 = (u2 == NULL ? NULL : u2->Next);
    }
  }

  other->use_list = NULL;
}

// 重命名算法的递归辅助
void rename_pass_help(Value *rename_var_pointer, Stack *rename_var_stack,
                      dom_tree *cur_bblock) {
  int num_of_def = 0;
  // printf("%s\n", cur_bblock->bblock_node->bblock_head->label->name);
  ListFirst(cur_bblock->bblock_node->bblock_head->inst_list, false);
  void *element;
  // 遍历当前bblock的instruction找出赋值语句
  while (ListNext(cur_bblock->bblock_node->bblock_head->inst_list, &element)) {
    // 如果是赋值语句则将操作数放在栈顶 用于后续的替换
    if (((Instruction *)element)->opcode == StoreOP &&
        user_get_operand_use(((User *)element), 0)->Val == rename_var_pointer) {
      // printf("store %s\n",
      //        user_get_operand_use(((User *)element), 0)->Val->name);
      num_of_def++;
      StackPush(rename_var_stack,
                user_get_operand_use(((User *)element), 1)->Val);
    } else if ((((Instruction *)element)->opcode == PhiFuncOp &&
                user_get_operand_use(((User *)element), 1)->Val ==
                    rename_var_pointer)) {
      // 如果是phi函数语句 因为phi函数语句和store语句同样是对变量的定义语句
      // 将phi函数instruction对应的value入栈
      // printf("store %s\n",
      //        user_get_operand_use(((User *)element), 1)->Val->name);
      num_of_def++;
      StackPush(rename_var_stack, element);
    } else if ((((Instruction *)element)->opcode == LoadOP &&
                user_get_operand_use(((User *)element), 0)->Val ==
                    rename_var_pointer)) {
      // printf("load %s\n",
      //        user_get_operand_use(((User *)element), 0)->Val->name);
      // 取出栈顶元素
      void *stack_top_var;
      StackTop(rename_var_stack, &stack_top_var);

      // 使用栈顶Value替换使用当前instruction的value
      replace_use_other_by_self(stack_top_var, element);
    }
  }

  // 遍历邻接边集合 修改phi函数中的参数
  void *neighbor_bblock;
  HashSetFirst(cur_bblock->bblock_node->edge_list);
  while ((neighbor_bblock = HashSetNext(cur_bblock->bblock_node->edge_list)) !=
         NULL) {
    void *neighbor_bblock_ins = NULL;
    ListFirst(((HeadNode *)neighbor_bblock)->bblock_head->inst_list, false);
    // 向前走一步跳过label的instruciton
    ListNext(((HeadNode *)neighbor_bblock)->bblock_head->inst_list,
             &neighbor_bblock_ins);

    // 三种情况同时满足则说明邻接bblock中需要修改含有该指针指向内存变量的phi函数
    while (ListNext(((HeadNode *)neighbor_bblock)->bblock_head->inst_list,
                    &neighbor_bblock_ins)) {
      if (((Instruction *)neighbor_bblock_ins)->opcode == PhiFuncOp &&
          user_get_operand_use(((User *)neighbor_bblock_ins), 1)->Val ==
              rename_var_pointer) {
        void *stack_top_var;
        StackTop(rename_var_stack, &stack_top_var);
        HashMapPut(user_get_operand_use(((User *)neighbor_bblock_ins), 0)
                       ->Val->pdata->phi_func_pdata.phi_value,
                   strdup(cur_bblock->bblock_node->bblock_head->label->name),
                   stack_top_var);
        break;
      }
    }
  }

  // 遍历dom_tree中的的孩子节点
  ListFirst(cur_bblock->child, false);
  void *child_node;
  while (ListNext(cur_bblock->child, &child_node)) {
    rename_pass_help(rename_var_pointer, rename_var_stack,
                     (dom_tree *)child_node);
  }

  // 该基本块中的将定义全部出栈
  for (int i = 0; i < num_of_def; i++) {
    StackPop(rename_var_stack);
  }
}

// 重命名算法
void rename_pass(Function *self) {
  int num_of_block = self->num_of_block;
  BasicBlock *entry_bblock = self->entry_bblock;

  // 遍历入口基本块找出allocate语句
  void *bblock_ins;
  ListFirst(entry_bblock->inst_list, false);
  int memory_iter = 0;

  while (ListNext(entry_bblock->inst_list, &bblock_ins)) {
    memory_iter++;
    if (((Instruction *)bblock_ins)->opcode == AllocateOP) {
      // 指向变量的指针 同时也是store_ins的第一个use对象
      Value *cur_rename_var = (Value *)bblock_ins;
      // 遍历树结构了
      Stack *rename_var_stack = StackInit();
      StackSetClean(rename_var_stack, CleanObject);
      rename_pass_help(cur_rename_var, rename_var_stack, dom_tree_root);
      ListFirst(entry_bblock->inst_list, false);
      for (int i = 0; i < memory_iter; i++) {
        ListNext(entry_bblock->inst_list, &bblock_ins);
      }
    } else if (memory_iter > 1) {
      break;
    }
  }
}

void delete_alloca_store_load_ins_pass(BasicBlock *self) {
  if (self != NULL && HashSetFind(bblock_pass_hashset, self) == false) {
    unsigned i = 0;
    void *element;
    ListFirst(self->inst_list, false);
    ListSetClean(self->inst_list, CommonCleanInstruction);
    while (i != ListSize(self->inst_list)) {
      ListGetAt(self->inst_list, i, &element);
      switch (((Instruction *)element)->opcode) {
        case AllocateOP:
          ListRemove(self->inst_list, i);
          break;
        case StoreOP:
          ListRemove(self->inst_list, i);
          break;
        case LoadOP:
          ListRemove(self->inst_list, i);
          break;
        case PhiFuncOp:
          memset(user_get_operand_use((User *)self, 1), 0, sizeof(Use));
          ((Instruction *)element)->user.num_oprands--;
          i++;
          break;
        default:
          i++;
          break;
      }
    }
    HashSetAdd(bblock_pass_hashset, self);
    delete_alloca_store_load_ins_pass(self->true_bblock);
    delete_alloca_store_load_ins_pass(self->false_bblock);
  }
}

// 生成支配树的pass和插入phi节点的pass
void bblock_to_dom_graph_pass(Function *self) {
  int num_of_block = self->num_of_block;
  // // 设置支配树对应图的邻接表头
  // hashset_init(&(graph_head_set));
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

  dom_tree_root = (dom_tree *)malloc(sizeof(dom_tree));
  dom_tree_root->bblock_node = init_headnode;
  dom_tree_root->child = ListInit();
  ListSetClean(dom_tree_root->child, CleanObject);

  dom_relation_pass();

  insert_phi_func_pass(self);

  printf("\n");

  // if (freopen("instruction_list.txt", "w", stdout) == NULL) {
  //   fprintf(stderr, "打开文件失败！");
  //   exit(-1);
  // }

  // 打印表的表头信息
  printf("\t%s\tnumber: %20s \t%25s \t%10s\n", "labelID", "opcode", "name",
         "use");
  // 打印当前函数的基本块
  print_bblock_pass(cur_func->entry_bblock);
  printf("\n\n");
  // 清空哈希表 然后重新初始化供后面使用
  HashSetDeinit(bblock_pass_hashset);
  bblock_pass_hashset = NULL;
  hashset_init(&(bblock_pass_hashset));

  printf("after rename pass and delete alloca store load\n");
  rename_pass(cur_func);

  // 删除alloca store load语句
  delete_alloca_store_load_ins_pass(cur_func->entry_bblock);
  // 清空哈希表 然后重新初始化供后面使用
  HashSetDeinit(bblock_pass_hashset);
  bblock_pass_hashset = NULL;
  hashset_init(&(bblock_pass_hashset));

  // 打印表的表头信息
  printf("\t%s\tnumber: %20s \t%25s \t%10s\n", "labelID", "opcode", "name",
         "use");
  // 打印当前函数的基本块
  print_bblock_pass(cur_func->entry_bblock);
  // 清空哈希表 然后重新初始化供后面使用
  HashSetDeinit(bblock_pass_hashset);
  bblock_pass_hashset = NULL;
  hashset_init(&(bblock_pass_hashset));
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
      printf("\t%10s, %10s",
             user_get_operand_use(((User *)element), 0)->Val->name,
             user_get_operand_use(((User *)element), 1)->Val->name);
      if (((Instruction *)element)->opcode == PhiFuncOp) {
        // printf(" %u", HashMapSize(user_get_operand_use(((User *)element), 0)
        //                               ->Val->pdata->phi_func_pdata.phi_value));

        Pair *ptr_pair;
        HashMapFirst(user_get_operand_use(((User *)element), 0)
                         ->Val->pdata->phi_func_pdata.phi_value);
        while ((ptr_pair = HashMapNext(
                    user_get_operand_use(((User *)element), 0)
                        ->Val->pdata->phi_func_pdata.phi_value)) != NULL) {
          printf("\tbblock: %s value: %s, ", (char *)(ptr_pair->key),
                 ((Value *)ptr_pair->value)->name);
        }
      }
      printf("\n");
    } else if (((Instruction *)element)->user.num_oprands == 1) {
      printf("\t%10s\n", user_get_operand_use(((User *)element), 0)->Val->name);
    } else if (((Instruction *)element)->user.num_oprands == 0) {
      printf("\t%10s\n", "null");
    }
  }
}

void print_bblock_pass(BasicBlock *self) {
  if (self != NULL && HashSetFind(bblock_pass_hashset, self) == false) {
    printf("\taddress:%p", self->label);
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

  while (ListNext(self, &element)) {
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
        TAC_OP cur_ins_opcode = ((Instruction *)element)->opcode;
        switch (cur_ins_opcode) {
          case GotoWithConditionOP:
            // printf("%s label %s ins is printed\n",
            // cur_bblock->label->name,
            //        op_string[((Instruction *)element)->opcode]);
            ListPushBack(cur_bblock->inst_list, element);
            if (!HashMapContain(
                    bblock_hashmap,
                    ((Value *)element)
                        ->pdata->condition_goto.true_goto_location->name)) {
              // 初始化要跳转的一个基本块
              BasicBlock *true_condition_block =
                  (BasicBlock *)malloc(sizeof(BasicBlock));
              bblock_init(true_condition_block, cur_func);
              ListPushBack(true_condition_block->father_bblock_list,
                           cur_bblock);
              true_condition_block->label =
                  ((Value *)element)->pdata->condition_goto.true_goto_location;
              HashMapPut(bblock_hashmap,
                         strdup(true_condition_block->label->name),
                         true_condition_block);
              cur_bblock->true_bblock = true_condition_block;
            } else {
              cur_bblock->true_bblock = HashMapGet(
                  bblock_hashmap,
                  ((Value *)element)
                      ->pdata->condition_goto.true_goto_location->name);
              ListPushBack(cur_bblock->true_bblock->father_bblock_list,
                           cur_bblock);
            }
            if (!HashMapContain(
                    bblock_hashmap,
                    ((Value *)element)
                        ->pdata->condition_goto.false_goto_location->name)) {
              // 初始化要跳转的一个基本块
              BasicBlock *false_condition_block =
                  (BasicBlock *)malloc(sizeof(BasicBlock));
              bblock_init(false_condition_block, cur_func);
              ListPushBack(false_condition_block->father_bblock_list,
                           cur_bblock);
              false_condition_block->label =
                  ((Value *)element)->pdata->condition_goto.false_goto_location;
              HashMapPut(bblock_hashmap,
                         strdup(false_condition_block->label->name),
                         false_condition_block);
              cur_bblock->false_bblock = false_condition_block;
            } else {
              cur_bblock->false_bblock = HashMapGet(
                  bblock_hashmap,
                  ((Value *)element)
                      ->pdata->condition_goto.false_goto_location->name);
              ListPushBack(cur_bblock->false_bblock->father_bblock_list,
                           cur_bblock);
            }
            break;

          case GotoOP:
            // printf("%s label %s ins is printed\n",
            // cur_bblock->label->name,
            //        op_string[((Instruction *)element)->opcode]);
            // 添加跳转语句在链条尾
            ListPushBack(cur_bblock->inst_list, element);
            if (!HashMapContain(
                    bblock_hashmap,
                    ((Value *)element)
                        ->pdata->no_condition_goto.goto_location->name)) {
              // 初始化要跳转的一个基本块
              BasicBlock *true_condition_block =
                  (BasicBlock *)malloc(sizeof(BasicBlock));
              bblock_init(true_condition_block, cur_func);
              ListPushBack(true_condition_block->father_bblock_list,
                           cur_bblock);
              true_condition_block->label =
                  ((Value *)element)->pdata->no_condition_goto.goto_location;
              HashMapPut(bblock_hashmap,
                         strdup(true_condition_block->label->name),
                         true_condition_block);
              cur_bblock->true_bblock = true_condition_block;
            } else {
              cur_bblock->true_bblock = HashMapGet(
                  bblock_hashmap,
                  ((Value *)element)
                      ->pdata->no_condition_goto.goto_location->name);
              ListPushBack(cur_bblock->true_bblock->father_bblock_list,
                           cur_bblock);
            }
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
          case AllocateOP:
            ListInsert(cur_func->entry_bblock->inst_list, 1, element);
            break;
          default:
            // printf("%s label %s ins push back\n",
            // cur_bblock->label->name,
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
