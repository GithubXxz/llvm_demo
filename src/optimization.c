#include "optimization.h"
#include "container/hash_map.h"
#include "container/list.h"
#include <string.h>

typedef struct _live_interval {
  unsigned begin;
  unsigned end;
} live_interval;

typedef struct _var_live_interval {
  char *self;
  List *this_var_discrete_live_interval;
  live_interval *this_var_total_live_interval;
} var_live_interval;

typedef struct _node_pair {
  char *key;
  HeadNode *value;
} node_pair;

static const int REGISTER_NUM = 3;

typedef enum _LOCATION { R1 = 1, R2, R3, MEMORY } LOCATION;

void calculate_live_use_def_by_graph(ALGraph *self) {
  for (int i = 0; i < self->node_num; i++) {
    ListFirst((self->node_set)[i]->bblock_head->inst_list, false);
    void *element;
    while (ListNext((self->node_set)[i]->bblock_head->inst_list, &element)) {
      if (((Instruction *)element)->opcode <= NULL_USED) {
        for (int j = 0; j < ((User *)element)->num_oprands; j++) {
          if (!HashSetFind(
                  (self->node_set)[i]->bblock_head->live_def,
                  user_get_operand_use((User *)element, j)->Val->name) &&
              (user_get_operand_use((User *)element, j)->Val->VTy->TID !=
                   ImmediateIntTyID &&
               user_get_operand_use((User *)element, j)->Val->VTy->TID !=
                   ImmediateFloatTyID)) {
#ifdef PRINT_OK
            printf("%s live use add %s\n",
                   (self->node_set)[i]->bblock_head->label->name,
                   user_get_operand_use((User *)element, j)->Val->name);
#endif
            HashSetAdd(
                (self->node_set)[i]->bblock_head->live_use,
                strdup(user_get_operand_use((User *)element, j)->Val->name));
          }
        }
        if (((Instruction *)element)->opcode < RETURN_USED) {
#ifdef PRINT_OK
          printf("%s live def add %s\n",
                 (self->node_set)[i]->bblock_head->label->name,
                 ((Value *)element)->name);
#endif
          HashSetAdd((self->node_set)[i]->bblock_head->live_def,
                     strdup(((Value *)element)->name));
        }
      }
    }
  }
#ifdef PRINT_OK
  printf("\n\n\n");
#endif
}

void calculate_live_in_out(ALGraph *self) {
  while (1) {
    bool no_liveout_changed = true;
    for (int i = self->node_num - 1; i >= 0; i--) {
      BasicBlock *cur_bblock = (self->node_set)[i]->bblock_head;
      HashSet *store_live_out = cur_bblock->live_out;
      cur_bblock->live_out = NULL;
      hashset_init_string(&(cur_bblock->live_out));

      HashMapFirst((self->node_set)[i]->edge_list);
      node_pair *element;
      while ((element = (node_pair *)HashMapNext(
                  (self->node_set)[i]->edge_list)) != NULL) {
        HashSet *union_set = HashSetUnion(cur_bblock->live_out,
                                          element->value->bblock_head->live_in);
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
    if (no_liveout_changed)
      break;
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

static var_live_interval *is_list_contain_item(List *self, char *item) {
  ListFirst(self, false);
  var_live_interval *element;
  while (ListNext(self, (void **)&element)) {
    if (!strcmp(element->self, item))
      return element;
  }
  return NULL;
}

void calculate_live_interval(ALGraph *self_cfg, Function *self_func) {
  unsigned ins_id_seed = 0;
  // 对所有的指令进行编号
  for (int i = 0; i < self_cfg->node_num; i++) {
    ListFirst((self_cfg->node_set)[i]->bblock_head->inst_list, false);
    Instruction *element;
    while (ListNext((self_cfg->node_set)[i]->bblock_head->inst_list,
                    (void **)&element))
      element->ins_id = ins_id_seed++;
  }

#ifdef PRINT_OK
  printf_cur_func_ins(self_func);
#endif

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
                     (void **)&cur_var_front_live_interval);
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
              ListSize((self_cfg->node_set)[i]->bblock_head->inst_list) - 1;
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
            ListSize(((self_cfg->node_set)[i]->bblock_head->inst_list)) - 1;
        ListPushFront(cur_var_live_interval->this_var_discrete_live_interval,
                      add_live_interval);
        ListPushBack(self_func->all_var_live_interval, cur_var_live_interval);
      }
    }

    ListFirst((self_cfg->node_set)[i]->bblock_head->inst_list, true);
    Instruction *element;
    while (ListReverseNext((self_cfg->node_set)[i]->bblock_head->inst_list,
                           (void **)&element)) {
      if (element->opcode < NULL_USED) {
        if (((Instruction *)element)->opcode < RETURN_USED) {
          var_live_interval *cur_var_live_interval = NULL;
          cur_var_live_interval = is_list_contain_item(
              self_func->all_var_live_interval, ((Value *)element)->name);
          if (cur_var_live_interval != NULL) {
            // 截断
            live_interval *cur_var_front_live_interval = NULL;
            ListGetFront(cur_var_live_interval->this_var_discrete_live_interval,
                         (void **)&cur_var_front_live_interval);
            cur_var_front_live_interval->begin = element->ins_id;
          }
        }

        for (int j = 0; j < ((User *)element)->num_oprands; j++) {
          // 当前live_out取出的变量是否已经存在在链条中了
          var_live_interval *cur_var_live_interval = NULL;
          Value *cur_handle = user_get_operand_use((User *)element, j)->Val;
          if ((cur_var_live_interval =
                   is_list_contain_item(self_func->all_var_live_interval,
                                        cur_handle->name)) != NULL) {
            // 存在的情况 先取出
            // 判断当前位置时候与取出的首活跃区间相邻来判断是新建还是延长
            live_interval *cur_var_front_live_interval = NULL;
            ListGetFront(cur_var_live_interval->this_var_discrete_live_interval,
                         (void **)&cur_var_front_live_interval);
            // 判断是否相邻
            if (element->ins_id <
                cur_var_front_live_interval->begin) { // 不相邻的情况 新建
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
  while (ListNext(self_func->all_var_live_interval, (void **)&element)) {
    live_interval *total_live_interval = NULL;
    ListGetFront(element->this_var_discrete_live_interval,
                 (void **)&total_live_interval);
    element->this_var_total_live_interval->begin = total_live_interval->begin;
    ListGetBack(element->this_var_discrete_live_interval,
                (void **)&total_live_interval);
    element->this_var_total_live_interval->end = total_live_interval->end;
  }

  // sort the list
  int num_of_var_live_interval = ListSize(self_func->all_var_live_interval);
  for (int i = 0; i < num_of_var_live_interval; i++) {
    var_live_interval *cur_index_var_live_interval = NULL;
    ListGetAt(self_func->all_var_live_interval, i,
              (void **)&cur_index_var_live_interval);
    for (int j = i + 1; j < num_of_var_live_interval; j++) {
      var_live_interval *cur_iter_var_live_interval = NULL;
      ListGetAt(self_func->all_var_live_interval, j,
                (void **)&cur_iter_var_live_interval);
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
}

void line_scan_register_allocation(Function *handle_func) {
  ALGraph *self_cfg = handle_func->self_cfg;
  HashMap *var_location = handle_func->var_localtion;
  var_live_interval *element;
  ListFirst(handle_func->all_var_live_interval, false);
#ifdef PRINT_OK
  while (ListNext(handle_func->all_var_live_interval, (void **)&element)) {
    printf("\tval:%s \tbegin:%d \tend:%d \n", element->self,
           element->this_var_total_live_interval->begin,
           element->this_var_total_live_interval->end);
  }
#endif

  List *active = ListInit();
  ListSetClean(active, CleanObject);
  // 0代表空闲 1代表被占用
  bool register_situation[10];
  for (int i = 0; i < 10; i++) {
    register_situation[i] = false;
  }

  if (ListSize(handle_func->all_var_live_interval) == 0) {
    return;
  }

  var_live_interval *cur_handle = NULL;
  ListFirst(handle_func->all_var_live_interval, false);

  // ListNext(handle_func->all_var_live_interval, (void **)&cur_handle);
  // if (cur_handle == NULL)
  //   return;
  // register_situation[R1] = true;
  // HashMapPut(var_location, strdup(cur_handle->self), (void *)(intptr_t)R1);
  // // printf("hashmap put %s\n", cur_handle->self);
  // ListPushBack(active, cur_handle);

  while (ListNext(handle_func->all_var_live_interval, (void **)&cur_handle)) {
    if (strstr(cur_handle->self, "global") ||
        strstr(cur_handle->self, "point")) {
      HashMapPut(var_location, strdup(cur_handle->self),
                 (void *)(intptr_t)MEMORY);
      continue;
    }
    // Expire OLD Intervals
    var_live_interval *iter_active = NULL;
    while (ListSize(active) != 0) {
      ListGetFront(active, (void **)&iter_active);
      if (iter_active->this_var_total_live_interval->end >=
          cur_handle->this_var_total_live_interval->begin)
        break;
      // release the register
      LOCATION cur_var_location =
          (LOCATION)(intptr_t)HashMapGet(var_location, iter_active->self);
      register_situation[cur_var_location] = false;
      ListPopFront(active);
    }
    iter_active = NULL;

    if (ListSize(active) == REGISTER_NUM) {
      // SpillATINTERVAL
      var_live_interval *active_tail_live_interval = NULL;
      ListGetBack(active, (void **)&active_tail_live_interval);
      if (active_tail_live_interval->this_var_total_live_interval->end >
          cur_handle->this_var_total_live_interval->end) {
        LOCATION cur_spill_var_location = (LOCATION)(intptr_t)HashMapGet(
            var_location, active_tail_live_interval->self);
        LOCATION cur_add_var_location = cur_spill_var_location;
        cur_spill_var_location = MEMORY;
        HashMapPut(var_location, strdup(active_tail_live_interval->self),
                   (void *)(intptr_t)cur_spill_var_location);
        HashMapPut(var_location, strdup(cur_handle->self),
                   (void *)(intptr_t)cur_add_var_location);
        ListPopBack(active);
        int list_size = ListSize(active);
        int i = 0;
        for (; i < list_size; i++) {
          var_live_interval *iter_active_live_interval = NULL;
          ListGetAt(active, i, (void **)&iter_active_live_interval);
          if (cur_handle->this_var_total_live_interval->end <
              iter_active_live_interval->this_var_total_live_interval->end) {
            ListInsert(active, i, cur_handle);
            break;
          }
        }
        if (i == list_size)
          ListPushBack(active, cur_handle);
      } else {
        LOCATION cur_add_var_location = MEMORY;
        HashMapPut(var_location, strdup(cur_handle->self),
                   (void *)(intptr_t)cur_add_var_location);
      }
    } else {
      for (int i = 1; i <= REGISTER_NUM; i++) {
        if (!register_situation[i]) {
          LOCATION cur_add_var_location = i;
          HashMapPut(var_location, strdup(cur_handle->self),
                     (void *)(intptr_t)cur_add_var_location);
          int j = 0;
          int list_size = ListSize(active);
          for (; j < list_size; j++) {
            var_live_interval *iter_active_live_interval = NULL;
            ListGetAt(active, j, (void **)&iter_active_live_interval);
            if (cur_handle->this_var_total_live_interval->end <
                iter_active_live_interval->this_var_total_live_interval->end) {
              ListInsert(active, j, cur_handle);
              break;
            }
          }
          if (j == list_size)
            ListPushBack(active, cur_handle);

          register_situation[i] = true;
          break;
        }
      }
    }
  }

  printf("\n");
  Pair *ptr_pair;
  HashMapFirst(var_location);
  if (HashMapSize(var_location) !=
      ListSize(handle_func->all_var_live_interval)) {
    printf("need to allocate register size is %d\n",
           ListSize(handle_func->all_var_live_interval));
    printf("indeed allocate register size is %d\n", HashMapSize(var_location));
    assert(0);
  };
}

void delete_non_used_var_pass(Function *handle_func) {
  ALGraph *self_cfg = handle_func->self_cfg;

  bool is_over = false;
  while (!is_over) {
    is_over = true;
    for (int i = 0; i < self_cfg->node_num; i++) {
      List *cur_handle = (self_cfg->node_set)[i]->bblock_head->inst_list;
      ListSetClean(cur_handle, CommonCleanInstruction);
      Instruction *element;
      int i = 0;
      while (i != ListSize(cur_handle)) {
        ListGetAt(cur_handle, i, (void *)&element);
        if (element->opcode < RETURN_USED &&
            ((Value *)element)->use_list == NULL) {
          ListRemove(cur_handle, i);
          is_over = false;
        } else
          i++;
      }
    }
  }
}