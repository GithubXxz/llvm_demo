#include "Pass.h"

#include <stdarg.h>  //变长参数函数所需的头文件
#include <stdio.h>

typedef struct _dom_tree {
  List *child;
  HeadNode *bblock_node;  // 分管的basicblock
} dom_tree;

typedef struct _copy_pair {
  Value *src;
  Value *dest;
} copy_pair;

typedef struct _live_interval {
  unsigned begin;
  unsigned end;
} live_interval;

typedef struct _var_live_interval {
  char *self;
  List *this_var_discrete_live_interval;
  live_interval *this_var_total_live_interval;
} var_live_interval;

static const int REGISTER_NUM = 3;

typedef enum _LOCATION { R1 = 1, R2, R3, MEMORY } LOCATION;

char *op_string[] = {"DefaultOP",
                     "AddOP",
                     "SubOP",
                     "MulOP",
                     "DivOP",
                     "EqualOP",
                     "NotEqualOP",
                     "GreatThanOP",
                     "LessThanOP",
                     "GreatEqualOP",
                     "LessEqualOP",
                     "AssignOP",
                     "PhiAssignOp",
                     "GetelementptrOP",
                     "CallWithReturnValueOP",
                     "LoadOP",
                     "ReturnOP",
                     "AllocateOP",
                     "StoreOP",
                     "GotoWithConditionOP",
                     "ParamOP",
                     "GotoOP",
                     "CallOP",
                     "LabelOP",
                     "FuncLabelOP",
                     "FuncEndOP",
                     "PhiFuncOp",
                     "InitArgO"};

static char *location_string[] = {"null", "R1", "R2", "R3", "M"};

extern List *func_list;

extern HashMap *bblock_hashmap;

extern HashSet *bblock_pass_hashset;

extern HashMap *bblock_to_dom_graph_hashmap;

static Function *cur_func = NULL;

static ALGraph *graph_for_dom_tree = NULL;

static dom_tree *dom_tree_root = NULL;

static int phi_var_seed = 1;   // 用于phi函数产生变量的名字
static int temp_var_seed = 1;  // 用于标识变量的名字

// 通过bblock的名字获得bblock的指针
BasicBlock *name_get_bblock(char *name) {
  return (BasicBlock *)HashMapGet(bblock_hashmap, name);
}

// void dom_tree_pass(List *self) {
//   int i = 0;
//   // printf("\nbegin print the instruction: \n");
//   void *element;
//   ListFirst(self, false);
//   while (ListNext(self, &element)) {
//     // 打印出每条instruction的res的名字信息
//     printf("%9s\t     %d: %20s ", "", i++,
//            op_string[((Instruction *)element)->opcode]);
//     printf("\t%25s ", ((Value *)element)->name);
//     if (((Instruction *)element)->user.num_oprands == 2) {
//       printf("\t%10s, %10s\n",
//              user_get_operand_use(((User *)element), 0)->Val->name,
//              user_get_operand_use(((User *)element), 1)->Val->name);
//     } else if (((Instruction *)element)->user.num_oprands == 1) {
//       printf("\t%10s\n", user_get_operand_use(((User *)element),
//       0)->Val->name);
//     } else if (((Instruction *)element)->user.num_oprands == 0) {
//       printf("\t%10s\n", "null");
//     }
//   }
// }

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
  printf("打印基本块之间的支配关系\n");
  // 打印入口节点的支配节点信息和地址
  printf("node entry dom ");
  for (int j = 0; j < node_num; j++) {
    printf("%s|%p,", graph_for_dom_tree->node_set[j]->bblock_head->label->name,
           graph_for_dom_tree->node_set[j]);
  }
  printf("\n");

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

    // 将删除的边添加回来
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
    // 把没有被访问的节点添加到当前节点的支配节点
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

  // for (int i = 0; i < node_num; i++) {
  //   HashSetFirst(graph_for_dom_tree->node_set[i]->dom_set);
  //   void *element;
  //   printf("%s dom node is ",
  //          graph_for_dom_tree->node_set[i]->bblock_head->label->name);
  //   while ((element = HashSetNext(graph_for_dom_tree->node_set[i]->dom_set))
  //   !=
  //          NULL) {
  //     printf("%s,", ((HeadNode *)element)->bblock_head->label->name);
  //   }
  //   printf("\n");
  // }

  printf("\n打印每个节点的立即支配节点\n");
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
          (HashSetSize(graph_for_dom_tree->node_set[j]->dom_set) <
           cur_idom_nodeset_num)) {
        cur_subscript = j;
        cur_idom_nodeset_num =
            HashSetSize(graph_for_dom_tree->node_set[j]->dom_set);
      }
    }

    graph_for_dom_tree->node_set[i]->idom_node =
        graph_for_dom_tree->node_set[cur_subscript];
    printf(
        "idom(%s) = %s\n",
        graph_for_dom_tree->node_set[i]->bblock_head->label->name,
        graph_for_dom_tree->node_set[i]->idom_node->bblock_head->label->name);
  }

  printf("\n打印支配树中的层级关系\n");
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

  printf("\n打印每个基本块的支配边界节点\n");

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

  // 打印每个节点和对应的支配边界
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

void find_non_locals_var_help(BasicBlock *self, HashSet *non_locals) {
  if (self != NULL && HashSetFind(bblock_pass_hashset, self) == false) {
    HashSet *killed = NULL;
    hashset_init(&(killed));
    ListFirst(self->inst_list, false);
    void *element = NULL;
    while (ListNext(self->inst_list, &element)) {
      if (((Instruction *)element)->opcode == LoadOP &&
          !HashSetFind(killed, user_get_operand_use(element, 0)->Val)) {
        HashSetAdd(non_locals, user_get_operand_use(element, 0)->Val);
      } else if (((Instruction *)element)->opcode == StoreOP) {
        HashSetAdd(killed, user_get_operand_use(element, 0)->Val);
      }
    }
    HashSetAdd(bblock_pass_hashset, self);
    find_non_locals_var_help(self->true_bblock, non_locals);
    find_non_locals_var_help(self->false_bblock, non_locals);
    HashSetDeinit(killed);
    killed = NULL;
  }
}

// hashset的并集会不会造成重大的内存泄漏？
// 找到要插入phi函数的bblock并且插入空的phi函数
void insert_phi_func_pass(Function *self) {
  int num_of_block = self->num_of_block;
  BasicBlock *entry_bblock = self->entry_bblock;
  void *bblock_ins = NULL;

  HashSet *non_locals = NULL;
  hashset_init(&(non_locals));

  find_non_locals_var_help(entry_bblock, non_locals);

  // void *temp_element = NULL;
  // HashSetFirst(non_locals);

  // while ((temp_element = HashSetNext(non_locals)) != NULL) {
  //   printf("%s 1111\n", ((Value *)temp_element)->name);
  // }

  // 清空哈希表 然后重新初始化供后面使用
  HashSetDeinit(bblock_pass_hashset);
  hashset_init(&(bblock_pass_hashset));

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

      // 找出对变量进行赋值的基本块
      for (int i = 1; i < num_of_block; i++) {
        if (graph_for_dom_tree->node_set[i]->is_visited) {
          graph_for_dom_tree->node_set[i]->is_visited = false;
          phi_insert_bblock =

              HashSetUnion(phi_insert_bblock,
                           graph_for_dom_tree->node_set[i]->dom_frontier_set);
        }
      }

      unsigned phi_insert_bblock_element_num = 0;
      // 寻找迭代支配边界
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
      // 插入的是bblock_ins作为指针指向的地址空间
      HeadNode *element;
      HashSetFirst(phi_insert_bblock);
      while ((element = HashSetNext(phi_insert_bblock)) != NULL &&
             HashSetFind(non_locals, bblock_ins)) {
        //// 打印要插入phi函数的基本块的名字信息
        // printf("%s\n", element->bblock_head->label->name);
        // 在内存中为phi函数分配空间
        // Value *cur_var = (Value *)malloc(sizeof(Value));
        // value_init(cur_var);
        // // 添加变量类型
        // cur_var->VTy->TID = PhiFuncTyID;
        // // 添加变量的名字
        // cur_var->name = strdup("phi_func");

        // // 初始化phi函数的hashmap
        // cur_var->pdata->phi_func_pdata.phi_value = HashMapInit();

        // HashMapSetHash(cur_var->pdata->phi_func_pdata.phi_value, HashKey);
        // HashMapSetCompare(cur_var->pdata->phi_func_pdata.phi_value,
        // CompareKey);
        // HashMapSetCleanKey(cur_var->pdata->phi_func_pdata.phi_value,
        //                    CleanHashMapKey);
        // HashMapSetCleanValue(cur_var->pdata->phi_func_pdata.phi_value,
        //                      CleanValue);

        // 给phi函数返回的 Value* 变量命名
        char temp_str[15];
        char text[10];
        sprintf(text, "%d", phi_var_seed);
        ++phi_var_seed;
        strcpy(temp_str, "\%phi_var");
        strcat(temp_str, text);
        // 创建phi函数语句 左值是被定义的变量 可以被引用
        // 第一个操作数是phi函数 第二个操作数是phi函数所对应的变量的指针
        Value *cur_ins = (Value *)ins_new_no_operator_v2(PhiFuncOp);
        // 添加变量类型
        cur_ins->VTy->TID =
            ((Value *)bblock_ins)->pdata->allocate_pdata.point_value->VTy->TID;
        // 添加变量的名字
        cur_ins->name = strdup(temp_str);
        cur_ins->pdata->phi_func_pdata.phi_pointer = (Value *)bblock_ins;
        hashmap_init(&(cur_ins->pdata->phi_func_pdata.phi_value));

        // printf("%s = phi,align 4\n", temp_str);

        ListInsert(element->bblock_head->inst_list, 1, cur_ins);
      }
    }
  }
  HashSetDeinit(non_locals);
  non_locals = NULL;
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

void rename_pass_help_new(HashMap *rename_var_stack_hashmap,
                          dom_tree *cur_bblock) {
  HashMap *num_of_var_def = NULL;
  hashmap_init(&num_of_var_def);
  // printf("%s\n", cur_bblock->bblock_node->bblock_head->label->name);
  ListFirst(cur_bblock->bblock_node->bblock_head->inst_list, false);
  void *element;
  // 遍历当前bblock的instruction找出赋值语句
  while (ListNext(cur_bblock->bblock_node->bblock_head->inst_list, &element)) {
    // 如果是赋值语句则将操作数放在栈顶 用于后续的替换
    if ((((Instruction *)element)->opcode == StoreOP ||
         ((Instruction *)element)->opcode == InitArgOP) &&
        user_get_operand_use(element, 0)->Val->VTy->TID != ArrayTyID) {
      if (HashMapContain(
              num_of_var_def,
              user_get_operand_use(((User *)element), 0)->Val->name)) {
        (*((int *)HashMapGet(
            num_of_var_def,
            user_get_operand_use(((User *)element), 0)->Val->name)))++;
      } else {
        int *num_of_this_var = (int *)malloc(sizeof(int));
        *num_of_this_var = 1;
        HashMapPut(
            num_of_var_def,
            strdup(user_get_operand_use(((User *)element), 0)->Val->name),
            num_of_this_var);
      }
      puts(user_get_operand_use(((User *)element), 0)->Val->name);
      puts(user_get_operand_use(((User *)element), 1)->Val->name);
      StackPush(
          HashMapGet(rename_var_stack_hashmap,
                     user_get_operand_use(((User *)element), 0)->Val->name),
          user_get_operand_use(((User *)element), 1)->Val);
    } else if (((Instruction *)element)->opcode == PhiFuncOp) {
      if (HashMapContain(
              num_of_var_def,
              ((Value *)element)->pdata->phi_func_pdata.phi_pointer->name)) {
        (*((int *)HashMapGet(
            num_of_var_def,
            ((Value *)element)->pdata->phi_func_pdata.phi_pointer->name)))++;
      } else {
        int *num_of_this_var = (int *)malloc(sizeof(int));
        *num_of_this_var = 1;
        HashMapPut(
            num_of_var_def,
            strdup(((Value *)element)->pdata->phi_func_pdata.phi_pointer->name),
            num_of_this_var);
      }
      StackPush(
          HashMapGet(
              rename_var_stack_hashmap,
              ((Value *)element)->pdata->phi_func_pdata.phi_pointer->name),
          element);
    } else if (((Instruction *)element)->opcode == LoadOP &&
               user_get_operand_use(element, 0)->Val->VTy->TID != ArrayTyID) {
      void *stack_top_var;
      StackTop(
          HashMapGet(rename_var_stack_hashmap,
                     user_get_operand_use(((User *)element), 0)->Val->name),
          &stack_top_var);

      // 使用栈顶Value替换使用当前instruction的value
      replace_use_other_by_self(stack_top_var, element);
    }
  }

  // 遍历邻接边集合 修改phi函数中的参数
  void *neighbor_bblock = NULL;
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
      if (((Instruction *)neighbor_bblock_ins)->opcode == PhiFuncOp) {
        void *stack_top_var;
        StackTop(HashMapGet(rename_var_stack_hashmap,
                            ((Value *)neighbor_bblock_ins)
                                ->pdata->phi_func_pdata.phi_pointer->name),
                 &stack_top_var);
        // printf("%p:phinode %s add %s from bblock %s\n", neighbor_bblock_ins,
        //        ((Value *)neighbor_bblock_ins)->name,
        //        ((Value *)stack_top_var)->name,
        //        cur_bblock->bblock_node->bblock_head->label->name);
        HashMapPut(
            ((Value *)neighbor_bblock_ins)->pdata->phi_func_pdata.phi_value,
            strdup(cur_bblock->bblock_node->bblock_head->label->name),
            stack_top_var);
      }
    }
  }

  // 遍历dom_tree中的的孩子节点
  ListFirst(cur_bblock->child, false);
  void *child_node;
  // TODO有问题
  while (ListNext(cur_bblock->child, &child_node)) {
    rename_pass_help_new(rename_var_stack_hashmap, (dom_tree *)child_node);
  }

  Pair *ptr_pair;

  HashMapFirst(num_of_var_def);
  while ((ptr_pair = HashMapNext(num_of_var_def)) != NULL) {
    for (int i = 0; i < *((int *)ptr_pair->value); i++) {
      StackPop((Stack *)HashMapGet(rename_var_stack_hashmap,
                                   ((char *)ptr_pair->key)));
    }
  }

  HashMapDeinit(num_of_var_def);
}

// 重命名算法
void rename_pass(Function *self) {
  int num_of_block = self->num_of_block;
  BasicBlock *entry_bblock = self->entry_bblock;

  // 遍历入口基本块找出allocate语句
  void *bblock_ins;
  ListFirst(entry_bblock->inst_list, false);
  int memory_iter = 0;

  // 初始化所有局部变量的变量栈
  HashMap *var_stack_hashmap = NULL;
  hashmap_init(&var_stack_hashmap);

  while (ListNext(entry_bblock->inst_list, &bblock_ins)) {
    if (((Instruction *)bblock_ins)->opcode == AllocateOP) {
      // 指向变量的指针 同时也是store_ins的第一个use对象
      Value *cur_rename_var = (Value *)bblock_ins;
      // 遍历树结构了
      Stack *rename_var_stack = StackInit();
      StackSetClean(rename_var_stack, CleanObject);
      HashMapPut(var_stack_hashmap, strdup(cur_rename_var->name),
                 rename_var_stack);
    }
  }

  rename_pass_help_new(var_stack_hashmap, dom_tree_root);

  HashMapDeinit(var_stack_hashmap);
  // Pair *ptr_pair;

  // while ((ptr_pair = HashMapNext(var_stack_hashmap)) != NULL) {
  //   printf("size is %d\n", StackSize((Stack *)ptr_pair->value));
  // }
  // printf("\n");

  // while (ListNext(entry_bblock->inst_list, &bblock_ins)) {
  //   memory_iter++;
  //   if (((Instruction *)bblock_ins)->opcode == AllocateOP) {
  //     // 指向变量的指针 同时也是store_ins的第一个use对象
  //     Value *cur_rename_var = (Value *)bblock_ins;
  //     // 遍历树结构了
  //     Stack *rename_var_stack = StackInit();
  //     StackSetClean(rename_var_stack, CleanObject);
  //     rename_pass_help(cur_rename_var, rename_var_stack, dom_tree_root);
  //     ListFirst(entry_bblock->inst_list, false);
  //     for (int i = 0; i < memory_iter; i++) {
  //       ListNext(entry_bblock->inst_list, &bblock_ins);
  //     }
  //   } else if (memory_iter > 1) {
  //     break;
  //   }
  // }
}

// 删除alloca store load语句
void delete_alloca_store_load_ins_pass(ALGraph *self) {
  for (int ii = 0; ii < self->node_num; ii++) {
    List *cur_handle = (self->node_set)[ii]->bblock_head->inst_list;

    ListSetClean(cur_handle, CommonCleanInstruction);
    void *element;
    int i = 0;
    while (i != ListSize(cur_handle)) {
      // printf("in %s: %p\n", self->label->name, self->label);
      ListGetAt(cur_handle, i, &element);
      switch (((Instruction *)element)->opcode) {
        case StoreOP:
          if (user_get_operand_use(element, 0)->Val->VTy->TID != ArrayTyID) {
            ListRemove(cur_handle, i);
          } else {
            i++;
          }
          break;
        case InitArgOP:
          if (user_get_operand_use(element, 0)->Val->VTy->TID != ArrayTyID) {
            ListRemove(cur_handle, i);
          } else {
            i++;
          }
          break;
        case LoadOP:
          if (user_get_operand_use(element, 0)->Val->VTy->TID != ArrayTyID) {
            ListRemove(cur_handle, i);
          } else {
            i++;
          }
          break;
        case PhiFuncOp:
          // memset(user_get_operand_use((User *)self, 1), 0, sizeof(Use));
          // ((Instruction *)element)->user.num_oprands--;
          i++;
          break;
        default:
          i++;
          break;
      }
    }
  }

  List *cur_handle = (self->node_set)[0]->bblock_head->inst_list;
  ListSetClean(cur_handle, CommonCleanInstruction);
  void *element;
  int i = 0;
  while (i != ListSize(cur_handle)) {
    // printf("in %s: %p\n", self->label->name, self->label);
    ListGetAt(cur_handle, i, &element);
    if (((Instruction *)element)->opcode == AllocateOP &&
        ((Value *)element)->VTy->TID != ArrayTyID) {
      ListRemove(cur_handle, i);
    } else {
      i++;
    }
  }

  // if (self != NULL && HashSetFind(bblock_pass_hashset, self) == false) {
  //   // printf("begin %s: %p\n", self->label->name, self->label);

  //   unsigned i = 0;
  //   void *element;
  //   ListFirst(self->inst_list, false);
  //   ListSetClean(self->inst_list, CommonCleanInstruction);

  //   while (i != ListSize(self->inst_list)) {
  //     // printf("in %s: %p\n", self->label->name, self->label);
  //     ListGetAt(self->inst_list, i, &element);
  //     switch (((Instruction *)element)->opcode) {
  //       // case AllocateOP:
  //       //   if (((Value *)element)->VTy->TID != ArrayTyID) {
  //       //     ListRemove(self->inst_list, i);
  //       //   } else {
  //       //     i++;
  //       //   }
  //       //   break;
  //       case StoreOP:
  //         if (user_get_operand_use(element, 0)->Val->VTy->TID != ArrayTyID) {
  //           ListRemove(self->inst_list, i);
  //         } else {
  //           i++;
  //         }
  //         break;
  //       case LoadOP:
  //         if (user_get_operand_use(element, 0)->Val->VTy->TID != ArrayTyID) {
  //           ListRemove(self->inst_list, i);
  //         } else {
  //           i++;
  //         }
  //         break;
  //       case PhiFuncOp:
  //         // memset(user_get_operand_use((User *)self, 1), 0, sizeof(Use));
  //         // ((Instruction *)element)->user.num_oprands--;
  //         i++;
  //         break;
  //       default:
  //         i++;
  //         break;
  //     }
  //   }

  //   // if (entry->false_bblock != false_situation ||
  //   //     entry->true_bblock != true_situation) {
  //   //   printf("!11\n");
  //   // }

  //   HashSetAdd(bblock_pass_hashset, self);

  //   delete_alloca_store_load_ins_pass(self->true_bblock);

  //   delete_alloca_store_load_ins_pass(self->false_bblock);
  // }
}

void printf_cur_func_ins(Function *self) {
  // 打印表的表头信息
  printf("\t%s\tnumber: %20s \t%25s \t%10s\n", "labelID", "opcode", "name",
         "use");
  // 打印当前函数的基本块
  print_bblock_pass(self->entry_bblock);
  printf("\n\n");
  // 清空哈希表 然后重新初始化供后面使用
  HashSetDeinit(bblock_pass_hashset);
  bblock_pass_hashset = NULL;
  hashset_init(&(bblock_pass_hashset));
}

void insert_copies_help(HashMap *insert_copies_stack_hashmap,
                        HashMap *num_of_var_def, dom_tree *cur_bblock) {
  // Pass One:Initialize the data structures
  HashSet *copy_set = NULL;
  hashset_init(&(copy_set));

  HashSet *worklist = NULL;
  hashset_init(&(worklist));

  // 伪代码中的map
  HashMap *var_replace_hashmap = NULL;
  hashmap_init(&var_replace_hashmap);
  // 伪代码中的used_by_another[src]
  HashSet *used_by_another = NULL;
  hashset_init(&used_by_another);

  // printf("cur bblock is %s\n",
  //        cur_bblock->bblock_node->bblock_head->label->name);

  // For all successors s of block
  void *neighbor_bblock = NULL;
  HashSetFirst(cur_bblock->bblock_node->edge_list);
  while ((neighbor_bblock = HashSetNext(cur_bblock->bblock_node->edge_list)) !=
         NULL) {
    void *neighbor_bblock_ins = NULL;
    ListFirst(((HeadNode *)neighbor_bblock)->bblock_head->inst_list, false);

    while (ListNext(((HeadNode *)neighbor_bblock)->bblock_head->inst_list,
                    &neighbor_bblock_ins)) {
      if (((Instruction *)neighbor_bblock_ins)->opcode == PhiFuncOp &&
          HashMapContain(
              ((Value *)neighbor_bblock_ins)->pdata->phi_func_pdata.phi_value,
              cur_bblock->bblock_node->bblock_head->label->name)) {
        copy_pair *cur_copy_pair = (copy_pair *)malloc(sizeof(copy_pair));
        cur_copy_pair->src = (Value *)(HashMapGet(
            ((Value *)neighbor_bblock_ins)->pdata->phi_func_pdata.phi_value,
            cur_bblock->bblock_node->bblock_head->label->name));
        cur_copy_pair->dest = (Value *)neighbor_bblock_ins;

        // printf("<%s,%s> is added\n", cur_copy_pair->src->name,
        //        cur_copy_pair->dest->name);

        HashSetAdd(copy_set, cur_copy_pair);
        HashMapPut(var_replace_hashmap, strdup(cur_copy_pair->src->name),
                   cur_copy_pair->src);
        HashMapPut(var_replace_hashmap, strdup(cur_copy_pair->dest->name),
                   cur_copy_pair->dest);
        HashSetAdd(used_by_another, cur_copy_pair->src);
      }
    }
  }
  copy_pair *cur_dest_used = NULL;
  HashSetFirst(copy_set);
  while ((cur_dest_used = HashSetNext(copy_set)) != NULL) {
    if (!HashSetFind(used_by_another, cur_dest_used->dest)) {
      HashSetAdd(worklist, cur_dest_used);
    }
  }
  HashSet *deleted_copy_pair = HashSetDifference(copy_set, worklist);

  HashSetDeinit(copy_set);
  copy_set = deleted_copy_pair;

  while ((HashSetSize(worklist) != 0) || (HashSetSize(copy_set) != 0)) {
    while (HashSetSize(worklist) != 0) {
      // Pick a <src,dest> from worklist
      HashSetFirst(worklist);
      copy_pair *cur_pick_pair = HashSetNext(worklist);
      HashSetRemove(worklist, cur_pick_pair);
      // If dest ∈ live_outb

      Value *phi_assign_ins = (Value *)ins_new_single_operator_v2(
          AssignOP, HashMapGet(var_replace_hashmap, cur_pick_pair->src->name));

      phi_assign_ins->name = strdup(cur_pick_pair->dest->name);
      phi_assign_ins->VTy->TID = cur_pick_pair->dest->VTy->TID;

      // phi_assign_ins->pdata->phi_replace_pdata.phi_replace_value =
      //     cur_pick_pair->dest;

      // Insert a copy operation from map[src] to dest at the end of cur_bblock
      ListInsert(cur_bblock->bblock_node->bblock_head->inst_list,
                 ListSize(cur_bblock->bblock_node->bblock_head->inst_list) - 1,
                 phi_assign_ins);

      // if (!strcmp(cur_bblock->bblock_node->bblock_head->label->name,
      //             "label3")) {
      //   Value *element = NULL;
      //   ListGetAt(cur_bblock->bblock_node->bblock_head->inst_list,
      // ListSize(cur_bblock->bblock_node->bblock_head->inst_list) -
      //             2, (void *)&element);
      //   printf("cur bblock tail name is %s\n", element->name);
      // }

      // map[src] <- dest
      HashMapPut(var_replace_hashmap, strdup(cur_pick_pair->src->name),
                 cur_pick_pair->dest);
      // If src is the name of a destination in copy_set
      // Add that copy to worklist
      HashSetFirst(copy_set);
      copy_pair *iter_copy_set;
      while ((iter_copy_set = HashSetNext(copy_set)) != NULL) {
        if (!strcmp(cur_pick_pair->src->name, iter_copy_set->dest->name))
          HashSetAdd(worklist, iter_copy_set);
      }
      HashSet *deleted_copy_pair_w = HashSetDifference(copy_set, worklist);
      HashSetDeinit(copy_set);
      copy_set = deleted_copy_pair_w;
    }
    if (HashSetSize(copy_set) != 0) {
      // Pick a <src,dest> from copy_set
      HashSetFirst(copy_set);
      copy_pair *cur_pick_pair = HashSetNext(copy_set);
      // copy_set <- copy_set - {<src,dest>}
      HashSetRemove(copy_set, cur_pick_pair);
      Instruction *assign_dest_to_temp =
          ins_new_single_operator_v2(AssignOP, cur_pick_pair->dest);
      char temp_str[20];
      char text[10];
      sprintf(text, "%d", temp_var_seed++);
      strcpy(temp_str, "phi_temp");
      strcat(temp_str, text);
      ((Value *)assign_dest_to_temp)->name = strdup(temp_str);
      ((Value *)assign_dest_to_temp)->VTy->TID = cur_pick_pair->dest->VTy->TID;
      ListInsert(cur_bblock->bblock_node->bblock_head->inst_list,
                 ListSize(cur_bblock->bblock_node->bblock_head->inst_list) - 1,
                 assign_dest_to_temp);

      // printf("insert %s =%s\n", ((Value *)assign_dest_to_temp)->name,
      //        cur_pick_pair->dest->name);

      HashMapPut(var_replace_hashmap, strdup(cur_pick_pair->dest->name),
                 (Value *)assign_dest_to_temp);
      HashSetAdd(worklist, cur_pick_pair);
    }
  }
}

void insert_copies(HashMap *insert_copies_stack_hashmap, dom_tree *cur_bblock) {
  HashMap *num_of_var_def = NULL;
  hashmap_init(&num_of_var_def);

  insert_copies_help(insert_copies_stack_hashmap, num_of_var_def, cur_bblock);

  // 遍历dom_tree中的的孩子节点
  ListFirst(cur_bblock->child, false);
  void *child_node;

  while (ListNext(cur_bblock->child, &child_node)) {
    insert_copies(insert_copies_stack_hashmap, (dom_tree *)child_node);
  }
}

void replace_phi_nodes(dom_tree *cur_bblock) {
  // Perform live analysis

  // For each variable v
  //     Stack[v]<-emptystack()
  HashMap *var_stack_hashmap = NULL;
  hashmap_init(&var_stack_hashmap);
  insert_copies(var_stack_hashmap, cur_bblock);
}

void calculate_live_use_def_by_graph(ALGraph *self) {
  for (int i = 0; i < self->node_num; i++) {
    ListFirst((self->node_set)[i]->bblock_head->inst_list, false);
    void *element;
    while (ListNext((self->node_set)[i]->bblock_head->inst_list, &element)) {
      if (((Instruction *)element)->opcode <= 18) {
        for (int j = 0; j < ((User *)element)->num_oprands; j++) {
          if (!HashSetFind(
                  (self->node_set)[i]->bblock_head->live_def,
                  user_get_operand_use((User *)element, j)->Val->name) &&
              (user_get_operand_use((User *)element, j)->Val->VTy->TID !=
                   ImmediateIntTyID &&
               user_get_operand_use((User *)element, j)->Val->VTy->TID !=
                   ImmediateFloatTyID)) {
            printf("%s live use add %s\n",
                   (self->node_set)[i]->bblock_head->label->name,
                   user_get_operand_use((User *)element, j)->Val->name);
            HashSetAdd(
                (self->node_set)[i]->bblock_head->live_use,
                strdup(user_get_operand_use((User *)element, j)->Val->name));
          }
        }
        if (((Instruction *)element)->opcode < RETURN_USED) {
          printf("%s live def add %s\n",
                 (self->node_set)[i]->bblock_head->label->name,
                 ((Value *)element)->name);
          HashSetAdd((self->node_set)[i]->bblock_head->live_def,
                     strdup(((Value *)element)->name));
        }
      }
    }
  }
  printf("\n\n\n");
}

void calculate_live_in_out(ALGraph *self) {
  while (1) {
    bool no_liveout_changed = true;
    for (int i = self->node_num - 1; i >= 0; i--) {
      BasicBlock *cur_bblock = (self->node_set)[i]->bblock_head;
      HashSet *store_live_out = cur_bblock->live_out;
      cur_bblock->live_out = NULL;
      hashset_init_string(&(cur_bblock->live_out));

      HashSetFirst((self->node_set)[i]->edge_list);
      HeadNode *element;
      while ((element = HashSetNext((self->node_set)[i]->edge_list)) != NULL) {
        HashSet *union_set =
            HashSetUnion(cur_bblock->live_out, element->bblock_head->live_in);
        HashSetDeinit(cur_bblock->live_out);
        cur_bblock->live_out = union_set;
      }

      HashSet *out_del_def =
          HashSetDifference(cur_bblock->live_out, cur_bblock->live_def);
      HashSet *res_union_use = HashSetUnion(out_del_def, cur_bblock->live_use);
      HashSetDeinit(out_del_def);
      HashSetDeinit(cur_bblock->live_in);
      cur_bblock->live_in = res_union_use;

      HashSet *intersert =
          HashSetIntersect(store_live_out, cur_bblock->live_out);
      unsigned intersertsize = HashSetSize(intersert);
      if (intersertsize != HashSetSize(cur_bblock->live_out) ||
          intersertsize != HashSetSize(store_live_out))
        no_liveout_changed = false;
      HashSetDeinit(intersert);
      HashSetDeinit(store_live_out);
    }
    if (no_liveout_changed) break;
  }

  /*
    打印全部的活跃信息
  */

  // for (int i = 0; i < self->node_num; i++) {
  //   HashSetFirst((self->node_set)[i]->bblock_head->live_def);
  //   char *live_def = NULL;
  //   printf("bblock %s's live def are ",
  //          (self->node_set)[i]->bblock_head->label->name);
  //   while ((live_def = HashSetNext(
  //               (self->node_set)[i]->bblock_head->live_def)) != NULL) {
  //     printf("\t%s", live_def);
  //   }
  //   printf("\n");

  //   HashSetFirst((self->node_set)[i]->bblock_head->live_use);
  //   char *live_use = NULL;
  //   printf("bblock %s's live use are ",
  //          (self->node_set)[i]->bblock_head->label->name);
  //   while ((live_use = HashSetNext(
  //               (self->node_set)[i]->bblock_head->live_use)) != NULL) {
  //     printf("\t%s", live_use);
  //   }
  //   printf("\n");
  // }
  // printf("\n\n\n");
  // for (int i = 0; i < self->node_num; i++) {
  //   HashSetFirst((self->node_set)[i]->bblock_head->live_out);
  //   char *live_out = NULL;
  //   printf("bblock %s's live out are ",
  //          (self->node_set)[i]->bblock_head->label->name);
  //   while ((live_out = HashSetNext(
  //               (self->node_set)[i]->bblock_head->live_out)) != NULL) {
  //     printf("\t%s", live_out);
  //   }
  //   printf("\n");

  //   HashSetFirst((self->node_set)[i]->bblock_head->live_in);
  //   char *live_in = NULL;
  //   printf("bblock %s's live in are ",
  //          (self->node_set)[i]->bblock_head->label->name);
  //   while ((live_in = HashSetNext((self->node_set)[i]->bblock_head->live_in))
  //   !=
  //          NULL) {
  //     printf("\t%s", live_in);
  //   }
  //   printf("\n");
  // }
}

var_live_interval *is_list_contain_item(List *self, char *item) {
  ListFirst(self, false);
  var_live_interval *element;
  while (ListNext(self, &element)) {
    if (!strcmp(element->self, item)) return element;
  }
  return NULL;
}

void calculate_live_interval(ALGraph *self_cfg, Function *self_func) {
  unsigned ins_id_seed = 0;
  // 对所有的指令进行编号
  for (int i = 0; i < self_cfg->node_num; i++) {
    ListFirst((self_cfg->node_set)[i]->bblock_head->inst_list, false);
    Instruction *element;
    while (ListNext((self_cfg->node_set)[i]->bblock_head->inst_list, &element))
      element->ins_id = ins_id_seed++;
  }

  printf_cur_func_ins(self_func);

  for (int i = self_cfg->node_num - 1; i >= 0; i--) {
    HashSetFirst((self_cfg->node_set)[i]->bblock_head->live_out);
    char *live_out_var = NULL;
    // iter the live_out_var of cur bblock
    while ((live_out_var = HashSetNext(
                (self_cfg->node_set)[i]->bblock_head->live_out)) != NULL) {
      // 当前live_out取出的变量是否已经存在在链条中了
      var_live_interval *cur_var_live_interval = NULL;

      if ((cur_var_live_interval = is_list_contain_item(
               self_func->all_var_live_interval, live_out_var)) != NULL) {
        // 存在的情况 先取出
        // 判断当前位置时候与取出的首活跃区间相邻来判断是新建还是延长
        live_interval *cur_var_front_live_interval = NULL;
        ListGetFront(cur_var_live_interval->this_var_discrete_live_interval,
                     &cur_var_front_live_interval);
        // 判断是否相邻
        if (((Instruction *)(self_cfg->node_set)[i + 1]->bblock_head->label)
                ->ins_id == cur_var_front_live_interval->begin) {
          cur_var_front_live_interval->begin =
              ((Instruction *)(self_cfg->node_set)[i]->bblock_head->label)
                  ->ins_id;
        } else {
          // 不相邻的情况 新建
          live_interval *add_live_interval =
              (live_interval *)malloc(sizeof(live_interval));
          add_live_interval->begin =
              ((Instruction *)(self_cfg->node_set)[i]->bblock_head->label)
                  ->ins_id;
          add_live_interval->end =
              add_live_interval->begin +
              ListSize((Instruction *)(self_cfg->node_set)[i]
                           ->bblock_head->inst_list) -
              1;
          ListPushFront(cur_var_live_interval->this_var_discrete_live_interval,
                        add_live_interval);
        }
      } else {
        cur_var_live_interval =
            (var_live_interval *)malloc(sizeof(var_live_interval));
        cur_var_live_interval->self = strdup(live_out_var);
        cur_var_live_interval->this_var_discrete_live_interval = ListInit();
        cur_var_live_interval->this_var_total_live_interval =
            (live_interval *)malloc(sizeof(live_interval));
        // 不相邻的情况 新建
        live_interval *add_live_interval =
            (live_interval *)malloc(sizeof(live_interval));
        add_live_interval->begin =
            ((Instruction *)(self_cfg->node_set)[i]->bblock_head->label)
                ->ins_id;
        add_live_interval->end =
            add_live_interval->begin +
            ListSize(((Instruction *)(self_cfg->node_set)[i]
                          ->bblock_head->inst_list)) -
            1;
        ListPushFront(cur_var_live_interval->this_var_discrete_live_interval,
                      add_live_interval);
        ListPushBack(self_func->all_var_live_interval, cur_var_live_interval);
      }
    }

    ListFirst((self_cfg->node_set)[i]->bblock_head->inst_list, true);
    Instruction *element;
    while (ListReverseNext((self_cfg->node_set)[i]->bblock_head->inst_list,
                           &element)) {
      if (element->opcode < NULL_USED) {
        if (((Instruction *)element)->opcode < RETURN_USED) {
          var_live_interval *cur_var_live_interval = NULL;
          cur_var_live_interval = is_list_contain_item(
              self_func->all_var_live_interval, ((Value *)element)->name);
          if (cur_var_live_interval != NULL) {
            // 截断
            live_interval *cur_var_front_live_interval = NULL;
            ListGetFront(cur_var_live_interval->this_var_discrete_live_interval,
                         &cur_var_front_live_interval);
            cur_var_front_live_interval->begin = element->ins_id;
          }
        }

        for (int j = 0; j < ((User *)element)->num_oprands; j++) {
          // 当前live_out取出的变量是否已经存在在链条中了
          var_live_interval *cur_var_live_interval = NULL;
          Value *cur_handle = user_get_operand_use((User *)element, j)->Val;
          if ((cur_var_live_interval = is_list_contain_item(
                   self_func->all_var_live_interval, cur_handle->name)) !=
              NULL) {
            // 存在的情况 先取出
            // 判断当前位置时候与取出的首活跃区间相邻来判断是新建还是延长
            live_interval *cur_var_front_live_interval = NULL;
            ListGetFront(cur_var_live_interval->this_var_discrete_live_interval,
                         &cur_var_front_live_interval);
            // 判断是否相邻
            if (element->ins_id <
                cur_var_front_live_interval->begin) {  // 不相邻的情况 新建
              live_interval *add_live_interval =
                  (live_interval *)malloc(sizeof(live_interval));
              add_live_interval->begin =
                  ((Instruction *)(self_cfg->node_set)[i]->bblock_head->label)
                      ->ins_id;
              add_live_interval->end = element->ins_id;
              ListPushFront(
                  cur_var_live_interval->this_var_discrete_live_interval,
                  add_live_interval);
            }
          } else if (cur_handle->VTy->TID != ImmediateFloatTyID &&
                     cur_handle->VTy->TID != ImmediateIntTyID) {
            cur_var_live_interval =
                (var_live_interval *)malloc(sizeof(var_live_interval));
            cur_var_live_interval->self =
                strdup(user_get_operand_use((User *)element, j)->Val->name);
            cur_var_live_interval->this_var_discrete_live_interval = ListInit();
            cur_var_live_interval->this_var_total_live_interval =
                (live_interval *)malloc(sizeof(live_interval));
            // 不相邻的情况 新建
            live_interval *add_live_interval =
                (live_interval *)malloc(sizeof(live_interval));
            add_live_interval->begin =
                ((Instruction *)(self_cfg->node_set)[i]->bblock_head->label)
                    ->ins_id;
            add_live_interval->end = element->ins_id;
            ListPushFront(
                cur_var_live_interval->this_var_discrete_live_interval,
                add_live_interval);
            ListPushBack(self_func->all_var_live_interval,
                         cur_var_live_interval);
          }
        }
      }
    }
  }

  var_live_interval *element;
  ListFirst(self_func->all_var_live_interval, false);
  while (ListNext(self_func->all_var_live_interval, &element)) {
    live_interval *total_live_interval = NULL;
    ListGetFront(element->this_var_discrete_live_interval,
                 &total_live_interval);
    element->this_var_total_live_interval->begin = total_live_interval->begin;
    ListGetBack(element->this_var_discrete_live_interval, &total_live_interval);
    element->this_var_total_live_interval->end = total_live_interval->end;
  }

  int num_of_var_live_interval = ListSize(self_func->all_var_live_interval);
  for (int i = 0; i < num_of_var_live_interval; i++) {
    var_live_interval *cur_index_var_live_interval = NULL;
    ListGetAt(self_func->all_var_live_interval, i,
              &cur_index_var_live_interval);
    for (int j = i + 1; j < num_of_var_live_interval; j++) {
      var_live_interval *cur_iter_var_live_interval = NULL;
      ListGetAt(self_func->all_var_live_interval, j,
                &cur_iter_var_live_interval);
      if (cur_index_var_live_interval->this_var_total_live_interval->begin >=
          cur_iter_var_live_interval->this_var_total_live_interval->begin) {
        ListSetAt(self_func->all_var_live_interval, i,
                  cur_iter_var_live_interval);
        ListSetAt(self_func->all_var_live_interval, j,
                  cur_index_var_live_interval);
        cur_index_var_live_interval = cur_iter_var_live_interval;
      }
    }
  }

  ListFirst(self_func->all_var_live_interval, false);
  while (ListNext(self_func->all_var_live_interval, &element)) {
    printf("\tval:%s \tbegin:%d \tend:%d \n", element->self,
           element->this_var_total_live_interval->begin,
           element->this_var_total_live_interval->end);
  }
}

// 生成支配树的pass和插入phi节点的pass和重命名函数的pass

void remove_bblock_phi_func_pass(ALGraph *self_cfg) {
  for (int i = 0; i < self_cfg->node_num; i++) {
    int iter_num = 0;
    Instruction *element = NULL;

    ListFirst((self_cfg->node_set)[i]->bblock_head->inst_list, false);
    ListSetClean((self_cfg->node_set)[i]->bblock_head->inst_list, CleanObject);
    while (
        ListNext((self_cfg->node_set)[i]->bblock_head->inst_list, &element)) {
      if (((Instruction *)element)->opcode == PhiFuncOp) {
        ListRemove((self_cfg->node_set)[i]->bblock_head->inst_list, iter_num);
        continue;
      }
      iter_num++;
    }
  }
}

// ins_list
void print_ins_pass(List *self) {
  int i = 0;
  // printf("\nbegin print the instruction: \n");
  void *element;
  ListFirst(self, false);
  while (ListNext(self, &element)) {
    // 打印出每条instruction的res的名字信息
    printf("%9s\t     %d: %20s ", "", ((Instruction *)element)->ins_id,
           op_string[((Instruction *)element)->opcode]);
    printf("\t%25s ", ((Value *)element)->name);

    if (((Instruction *)element)->opcode == PhiFuncOp) {
      printf("\tsize: %d ",
             HashMapSize(((Value *)element)->pdata->phi_func_pdata.phi_value));
      Pair *ptr_pair;
      HashMapFirst(((Value *)element)->pdata->phi_func_pdata.phi_value);
      while ((ptr_pair = HashMapNext(
                  ((Value *)element)->pdata->phi_func_pdata.phi_value)) !=
             NULL) {
        printf("\tbblock: %s value: %s, ", (char *)(ptr_pair->key),
               ((Value *)ptr_pair->value)->name);
      }
    } else if (((Instruction *)element)->user.num_oprands == 2) {
      printf("\t%10s, %10s",
             user_get_operand_use(((User *)element), 0)->Val->name,
             user_get_operand_use(((User *)element), 1)->Val->name);
    } else if (((Instruction *)element)->user.num_oprands == 1) {
      printf("\t%10s", user_get_operand_use(((User *)element), 0)->Val->name);
    } else if (((Instruction *)element)->user.num_oprands == 0) {
      printf("\t%10s", "null");
    }
    printf("\n");
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
  // 用来记录当前的正在处理的bblock
  BasicBlock *cur_bblock = NULL;
  // 记录前一条语句的opcode
  TAC_OP pre_op;
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

      // 插入函数链表中
      ListPushBack(func_list, cur_func);

      // 初始化entryLabel并且插入到函数的入口label
      ListNext(self, &element);

      cur_bblock = (BasicBlock *)malloc(sizeof(BasicBlock));
      bblock_init(cur_bblock, cur_func);
      cur_bblock->label = (Value *)element;

      // 设置当前的函数的入口基本块
      cur_func->entry_bblock = cur_bblock;
      ListPushBack(cur_bblock->inst_list, element);

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
          // TODO 修改代码逻辑 使用标志flag替换hashset
          // 先遍历一次建立所有基本块应该是更优而且易理解的算法
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
              char temp_str[30];
              strcpy(temp_str, "goto ");
              strcat(temp_str, end_bblock->label->name);
              Value *goto_end_bblock_ins =
                  (Value *)ins_new_no_operator_v2(GotoOP);
              goto_end_bblock_ins->name = strdup(temp_str);
              goto_end_bblock_ins->VTy->TID = GotoTyID;
              goto_end_bblock_ins->pdata->no_condition_goto.goto_location =
                  end_bblock->label;
              cur_bblock->true_bblock = end_bblock;
              ListPushBack(cur_bblock->inst_list, goto_end_bblock_ins);
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
      cur_func->num_of_block = num_of_block;
    }

    // // 打印当前函数的基本块
    // print_bblock_pass(cur_func->entry_bblock);
    // // 清空哈希表 然后重新初始化供后面使用
    // HashSetDeinit(bblock_pass_hashset);
    // hashset_init(&(bblock_pass_hashset));
  }
}

void delete_return_deadcode_pass(List *self) {
  void *element;
  ListFirst(self, true);
  ListSetClean(self, CommonCleanInstruction);
  unsigned i = 0;
  HashSet *reach_label = NULL;
  hashset_init(&reach_label);
  while (i != ListSize(self)) {
    ListGetAt(self, i, &element);
    switch (((Instruction *)element)->opcode) {
      case GotoOP:
        HashSetAdd(reach_label,
                   ((Value *)element)->pdata->no_condition_goto.goto_location);
        i++;
        while (ListGetAt(self, i, &element) &&
               (((Instruction *)element)->opcode == GotoOP ||
                ((Instruction *)element)->opcode == GotoWithConditionOP)) {
          ListRemove(self, i);
        }
        break;
      case GotoWithConditionOP:
        HashSetAdd(
            reach_label,
            ((Value *)element)->pdata->condition_goto.true_goto_location);
        HashSetAdd(
            reach_label,
            ((Value *)element)->pdata->condition_goto.false_goto_location);
        i++;
        while (ListGetAt(self, i, &element) &&
               (((Instruction *)element)->opcode == GotoOP ||
                ((Instruction *)element)->opcode == GotoWithConditionOP)) {
          ListRemove(self, i);
        }
        break;
      case ReturnOP:
        i++;
        while (ListGetAt(self, i, &element) &&
               (((Instruction *)element)->opcode != LabelOP &&
                ((Instruction *)element)->opcode != FuncEndOP)) {
          ListRemove(self, i);
        }
        break;
      case LabelOP:
        while (!HashSetFind(reach_label, (Value *)element) &&
               strcmp(((Value *)element)->name, "entry")) {
          ListRemove(self, i);
          while (ListGetAt(self, i, &element) &&
                 (((Instruction *)element)->opcode != LabelOP)) {
            ListRemove(self, i);
          }
        }
        i++;
        break;
      default:
        i++;
        break;
    }
  }
  ListSetClean(self, CleanObject);
}

void line_scan_register_allocation(ALGraph *self_cfg, Function *self_func,
                                   HashMap *var_location) {
  List *active = ListInit();
  ListSetClean(active, CleanObject);
  // 0代表空闲 1代表被占用
  bool register_situation[10];
  for (int i = 0; i < 10; i++) {
    register_situation[i] = false;
  }

  var_live_interval *cur_handle = NULL;
  ListFirst(self_func->all_var_live_interval, false);

  ListNext(self_func->all_var_live_interval, &cur_handle);
  if (cur_handle == NULL) return;
  LOCATION *cur_add_var_location = (LOCATION *)malloc(sizeof(LOCATION));
  *cur_add_var_location = 1;
  register_situation[1] = true;
  HashMapPut(var_location, strdup(cur_handle->self), cur_add_var_location);
  ListPushBack(active, cur_handle);

  while (ListNext(self_func->all_var_live_interval, &cur_handle)) {
    // Expire OLD Intervals
    var_live_interval *iter_active = NULL;
    while (ListSize(active) != 0) {
      ListGetFront(active, &iter_active);
      if (iter_active->this_var_total_live_interval->end >=
          cur_handle->this_var_total_live_interval->begin)
        break;
      LOCATION *cur_var_location =
          (LOCATION *)HashMapGet(var_location, iter_active->self);
      register_situation[*cur_var_location] = false;
      ListPopFront(active);
    }
    iter_active = NULL;

    if (ListSize(active) == REGISTER_NUM) {
      var_live_interval *active_tail_live_interval = NULL;
      ListGetBack(active, &active_tail_live_interval);
      if (active_tail_live_interval->this_var_total_live_interval->end >
          cur_handle->this_var_total_live_interval->end) {
        LOCATION *cur_spill_var_location = (LOCATION *)HashMapGet(
            var_location, active_tail_live_interval->self);
        LOCATION *cur_add_var_location = (LOCATION *)malloc(sizeof(LOCATION));
        *cur_add_var_location = *cur_spill_var_location;
        *cur_spill_var_location = MEMORY;
        HashMapPut(var_location, strdup(cur_handle->self),
                   cur_add_var_location);
        ListPopBack(active);
        for (int i = 0; i < ListSize(active); i++) {
          var_live_interval *iter_active_live_interval = NULL;
          ListGetAt(active, i, &iter_active_live_interval);
          if (cur_handle->this_var_total_live_interval->end <
              iter_active_live_interval->this_var_total_live_interval->end) {
            ListInsert(active, i, cur_handle);
            break;
          }
        }
      } else {
        LOCATION *cur_add_var_location = (LOCATION *)malloc(sizeof(LOCATION));
        *cur_add_var_location = MEMORY;
        HashMapPut(var_location, strdup(cur_handle->self),
                   cur_add_var_location);
      }
    } else {
      for (int i = 1; i <= REGISTER_NUM; i++) {
        if (!register_situation[i]) {
          LOCATION *cur_add_var_location = (LOCATION *)malloc(sizeof(LOCATION));
          *cur_add_var_location = i;
          HashMapPut(var_location, strdup(cur_handle->self),
                     cur_add_var_location);
          int j = 0;
          for (; j < ListSize(active); j++) {
            var_live_interval *iter_active_live_interval = NULL;
            ListGetAt(active, j, &iter_active_live_interval);
            if (cur_handle->this_var_total_live_interval->end <
                iter_active_live_interval->this_var_total_live_interval->end) {
              ListInsert(active, j, cur_handle);
              break;
            }
          }
          if (j == ListSize(active)) ListPushBack(active, cur_handle);

          register_situation[i] = true;
          break;
        }
      }
    }
  }
}

void register_replace(ALGraph *self_cfg, Function *self_func,
                      HashMap *var_location) {
  Pair *ptr_pair;
  HashMapFirst(var_location);
  while ((ptr_pair = HashMapNext(var_location)) != NULL) {
    printf("\tvar:%s\taddress:%s\n ", (char *)ptr_pair->key,
           location_string[*((LOCATION *)ptr_pair->value)]);
  }

  // for (int i = 0; i < self_cfg->node_num; i++) {
  //   int iter_num = 0;
  //   while (iter_num <
  //          ListSize((self_cfg->node_set)[i]->bblock_head->inst_list)) {
  //     Instruction *element = NULL;
  //     ListGetAt((self_cfg->node_set)[i]->bblock_head->inst_list, iter_num,
  //               &element);
  //     if (element->opcode < 19) {
  //       for (int j = 0; j < ((User *)element)->num_oprands; j++) {
  //         Value *cur_handle = user_get_operand_use((User *)element, j)->Val;
  //         // R replace name

  //         // M store load
  //       }

  //       if (((Instruction *)element)->opcode < RETURN_USED) {
  //         if (HashMapGet(var_location, ((Value *)element)->name)) {
  //           LOCATION cur_var_location = *(
  //               (LOCATION *)HashMapGet(var_location, ((Value
  //               *)element)->name));
  //           if (cur_var_location == MEMORY) {
  //             // 将当前语句改成store语句
  //           } else {
  //             char *temp_str = strdup(location_string[*((LOCATION
  //             *)HashMapGet(
  //                 var_location, ((Value *)element)->name))]);
  //             free(((Value *)element)->name);
  //             ((Value *)element)->name = temp_str;
  //           }
  //         } else {
  //           ListRemove((self_cfg->node_set)[i]->bblock_head->inst_list,
  //                      iter_num);
  //           continue;
  //         }
  //       }
  //     }
  //     iter_num++;
  //   }
  // }
}

void bblock_to_dom_graph_pass(Function *self) {
  int num_of_block = self->num_of_block;
  // // 设置支配树对应图的邻接表头
  // hashset_init(&(graph_head_set));
  graph_for_dom_tree = (ALGraph *)malloc(sizeof(ALGraph));
  graph_for_dom_tree->node_set =
      (HeadNode **)malloc(num_of_block * sizeof(HeadNode *));
  graph_for_dom_tree->node_num = num_of_block;
  self->self_cfg = graph_for_dom_tree;

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
  hashmap_init(&bblock_to_dom_graph_hashmap);

  // 打印邻接边
  for (int i = 0; i < num_of_block; i++) {
    HashSetFirst(graph_for_dom_tree->node_set[i]->edge_list);
    void *element;
    printf("%s edge is ",
           graph_for_dom_tree->node_set[i]->bblock_head->label->name);
    while ((element = HashSetNext(
                graph_for_dom_tree->node_set[i]->edge_list)) != NULL) {
      printf("%s ", ((HeadNode *)element)->bblock_head->label->name);
    }
    printf("\n");
  }

  // 初始化dom_tree树根
  dom_tree_root = (dom_tree *)malloc(sizeof(dom_tree));
  dom_tree_root->bblock_node = init_headnode;
  dom_tree_root->child = ListInit();
  ListSetClean(dom_tree_root->child, CleanObject);

  // 建立支配关系的函数
  dom_relation_pass();

  // 插入phi函数
  insert_phi_func_pass(self);

  printf("\n");

  printf_cur_func_ins(self);

  printf("begin rename pass and delete alloca,store,load instruction!\n");

  rename_pass(self);

  printf("rename pass over\n");

  // 删除alloca store load语句
  delete_alloca_store_load_ins_pass(graph_for_dom_tree);

  printf("delete alloca,store,load instruction over\n");

  // 清空哈希表 然后重新初始化供后面使用
  HashSetDeinit(bblock_pass_hashset);
  bblock_pass_hashset = NULL;
  hashset_init(&(bblock_pass_hashset));

  // if (freopen("instruction_list.txt", "w", stdout) == NULL) {
  //   fprintf(stderr, "打开文件失败！");
  //   exit(-1);
  // }

  printf_cur_func_ins(self);

  replace_phi_nodes(dom_tree_root);

  remove_bblock_phi_func_pass(graph_for_dom_tree);

  printf("\n\n\n");

  printf_cur_func_ins(self);

  calculate_live_use_def_by_graph(graph_for_dom_tree);

  calculate_live_in_out(graph_for_dom_tree);

  calculate_live_interval(graph_for_dom_tree, self);

  // <Value*,*LOCATION>
  HashMap *var_location = HashMapInit();
  hashmap_init(&var_location);

  line_scan_register_allocation(graph_for_dom_tree, self, var_location);

  register_replace(graph_for_dom_tree, self, var_location);
}
