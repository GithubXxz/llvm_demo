#include "optimization.h"
#include "Pass.h"
#include "bblock.h"
#include "c_container_auxiliary.h"
#include "container/hash_map.h"
#include "container/hash_set.h"
#include "container/list.h"
#include "container/tree_map.h"
#include "function.h"
#include "math/hash.h"
#include "type.h"
#include "use.h"
#include "user.h"
#include <malloc/_malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/_types/_intptr_t.h>

extern HashMap *constant_single_value_hashmap;

typedef struct _live_interval {
  unsigned begin;
  unsigned end;
} live_interval;

typedef struct _var_live_interval {
  char *self;
  List *this_var_discrete_live_interval;
  live_interval *this_var_total_live_interval;
} var_live_interval;

typedef struct _live_interval_pair {
  char *key;
  var_live_interval *value;
} live_interval_pair;

typedef struct _live_sorted_interval_pair {
  var_live_interval *key;
  void *value;
} live_sorted_interval_pair;

typedef struct _node_pair {
  char *key;
  HeadNode *value;
} node_pair;

// static const int REGISTER_NUM = 3;
// typedef enum _LOCATION { R1 = 1, R2, R3, MEMORY } LOCATION;

static const int REGISTER_NUM = 8;
typedef enum _LOCATION {
  ALLOC_R4 = 1,
  ALLOC_R5,
  ALLOC_R6,
  ALLOC_R8,
  ALLOC_R9,
  ALLOC_R10,
  ALLOC_R11,
  ALLOC_R12,
  MEMORY
} LOCATION;

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
  TIMER_BEGIN;
  unsigned ins_id_seed = 0;
  // 对所有的指令进行编号
  for (int i = 0; i < self_cfg->node_num; i++) {
    ListFirst((self_cfg->node_set)[i]->bblock_head->inst_list, false);
    Instruction *element;
    while (ListNext((self_cfg->node_set)[i]->bblock_head->inst_list,
                    (void **)&element))
      element->ins_id = ins_id_seed++;
  }
  TIMER_END("increment number for instruction");

#ifdef PRINT_OK
  printf_cur_func_ins(self_func);
#endif

  TIMER_BEGIN;
  HashMap *all_var_live_interval;
  hashmap_init(&all_var_live_interval);
  for (int i = self_cfg->node_num - 1; i >= 0; i--) {
    HashSetFirst((self_cfg->node_set)[i]->bblock_head->live_out);
    char *live_out_var_name = NULL;
    // iter the live_out_var of cur bblock
    while ((live_out_var_name = HashSetNext(
                (self_cfg->node_set)[i]->bblock_head->live_out)) != NULL) {
      // 当前live_out取出的变量是否已经存在在链条中了
      var_live_interval *cur_var_live_interval = NULL;

      if (HashMapContain(all_var_live_interval, live_out_var_name)) {
        cur_var_live_interval =
            HashMapGet(all_var_live_interval, live_out_var_name);
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
        cur_var_live_interval->self = strdup(live_out_var_name);
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
        HashMapPut(all_var_live_interval, strdup(live_out_var_name),
                   cur_var_live_interval);
      }
    }

    ListFirst((self_cfg->node_set)[i]->bblock_head->inst_list, true);
    Instruction *element;
    while (ListReverseNext((self_cfg->node_set)[i]->bblock_head->inst_list,
                           (void **)&element)) {
      if (element->opcode < NULL_USED) {
        if (((Instruction *)element)->opcode < RETURN_USED) {
          if (HashMapContain(all_var_live_interval, ((Value *)element)->name)) {
            var_live_interval *cur_var_live_interval =
                HashMapGet(all_var_live_interval, ((Value *)element)->name);
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
          if (HashMapContain(all_var_live_interval, cur_handle->name)) {
            // 存在的情况 先取出
            // 判断当前位置时候与取出的首活跃区间相邻来判断是新建还是延长
            cur_var_live_interval =
                HashMapGet(all_var_live_interval, cur_handle->name);
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
            HashMapPut(all_var_live_interval,
                       strdup(cur_var_live_interval->self),
                       cur_var_live_interval);
          }
        }
      }
    }
  }
  TIMER_END("increment number for instruction");

  live_interval_pair *element;
  HashMapFirst(all_var_live_interval);
  while ((element = (live_interval_pair *)HashMapNext(all_var_live_interval)) !=
         NULL) {
    live_interval *total_live_interval = NULL;
    ListGetFront(element->value->this_var_discrete_live_interval,
                 (void **)&total_live_interval);
    element->value->this_var_total_live_interval->begin =
        total_live_interval->begin;
    ListGetBack(element->value->this_var_discrete_live_interval,
                (void **)&total_live_interval);
    element->value->this_var_total_live_interval->end =
        total_live_interval->end;
    TreeMapPut(self_func->all_var_live_interval, element->value, NULL);
  }
}

void line_scan_register_allocation(Function *handle_func) {
  ALGraph *self_cfg = handle_func->self_cfg;
  HashMap *var_location = handle_func->var_localtion;
  live_sorted_interval_pair *element;
#ifdef PRINT_OK
  TreeMapFirst(handle_func->all_var_live_interval);
  while ((element = (live_sorted_interval_pair *)TreeMapNext(
              handle_func->all_var_live_interval)) != NULL) {
    printf("\tval:%s \tbegin:%d \tend:%d \n", element->key->self,
           element->key->this_var_total_live_interval->begin,
           element->key->this_var_total_live_interval->end);
  }
#endif

  List *active = ListInit();
  ListSetClean(active, CleanObject);
  // 0代表空闲 1代表被占用
  bool register_situation[10];
  for (int i = 0; i < 10; i++) {
    register_situation[i] = false;
  }

  TreeMapFirst(handle_func->all_var_live_interval);
  if (TreeMapSize(handle_func->all_var_live_interval) == 0) {
    return;
  }

  TreeMapFirst(handle_func->all_var_live_interval);

  // ListNext(handle_func->all_var_live_interval, (void **)&cur_handle);
  // if (cur_handle == NULL)
  //   return;
  // register_situation[R1] = true;
  // HashMapPut(var_location, strdup(cur_handle->self), (void *)(intptr_t)R1);
  // // printf("hashmap put %s\n", cur_handle->self);
  // ListPushBack(active, cur_handle);

  live_sorted_interval_pair *cur_handle_help;
  var_live_interval *cur_handle;
  TreeMapFirst(handle_func->all_var_live_interval);
  while ((cur_handle_help = (live_sorted_interval_pair *)TreeMapNext(
              handle_func->all_var_live_interval)) != NULL) {
    cur_handle = cur_handle_help->key;
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
      TreeMapSize(handle_func->all_var_live_interval)) {
    printf("need to allocate register size is %d\n",
           TreeMapSize(handle_func->all_var_live_interval));
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
      List *cur_handle_list = (self_cfg->node_set)[i]->bblock_head->inst_list;
      ListSetClean(cur_handle_list, CommonCleanInstruction);
      Instruction *element;
      int i = 0;
      ListNode *iter = cur_handle_list->data->head_;
      element = iter->element_;

      while (i < ListSize(cur_handle_list)) {
        if (element->opcode < RETURN_USED &&
            element->opcode != CallWithReturnValueOP &&
            ((Value *)element)->use_list == NULL) {
          delete_ins(cur_handle_list, &iter, &element);
          is_over = false;
        } else
          iter_next_ins(&iter, &i, &element);
      }
    }
  }
}

void immediate_num_calculate(Function *handle_func) {
  ALGraph *self_cfg = handle_func->self_cfg;

  for (int ii = 0; ii < self_cfg->node_num; ii++) {
    BasicBlock *cur_handle_bblock = (self_cfg->node_set)[ii]->bblock_head;
    List *cur_handle_list = cur_handle_bblock->inst_list;
    ListSetClean(cur_handle_list, CommonCleanInstruction);
    Instruction *element;
    int i = 0;
    ListNode *iter = cur_handle_list->data->head_;
    element = iter->element_;

    while (i < ListSize(cur_handle_list)) {
      if (element->user.num_oprands == 2) {
        Value *oprand01 = user_get_operand_use((User *)element, 0)->Val;
        Value *oprand02 = user_get_operand_use((User *)element, 1)->Val;
        if ((oprand01->VTy->TID == ImmediateIntTyID ||
             oprand01->VTy->TID == ImmediateFloatTyID) &&
            (oprand02->VTy->TID == ImmediateIntTyID ||
             oprand02->VTy->TID == ImmediateFloatTyID)) {

          Value *cur;
          TypeID cur_res_typeid = imm_res_type(oprand01, oprand02);
          char buffer[30];
          float const_float_value = 0.0f;
          int const_int_value = 0;

          if (cur_res_typeid == ImmediateFloatTyID) {
            switch (((Instruction *)element)->opcode) {
            case AddOP:
              const_float_value = oprand01->pdata->var_pdata.fVal +
                                  oprand02->pdata->var_pdata.fVal;
              sprintf(buffer, "%f", const_float_value);
              break;
            case SubOP:
              const_float_value = oprand01->pdata->var_pdata.fVal -
                                  oprand02->pdata->var_pdata.fVal;
              sprintf(buffer, "%f", const_float_value);
              break;
            case MulOP:
              const_float_value = oprand01->pdata->var_pdata.fVal *
                                  oprand02->pdata->var_pdata.fVal;
              sprintf(buffer, "%f", const_float_value);
              break;
            case DivOP:
              const_float_value = oprand01->pdata->var_pdata.fVal /
                                  oprand02->pdata->var_pdata.fVal;
              sprintf(buffer, "%f", const_float_value);
              break;

            case EqualOP:
              const_float_value = oprand01->pdata->var_pdata.fVal ==
                                  oprand02->pdata->var_pdata.fVal;
              sprintf(buffer, "%d", (int)const_float_value);
              break;

            case NotEqualOP:
              const_float_value = oprand01->pdata->var_pdata.fVal !=
                                  oprand02->pdata->var_pdata.fVal;
              sprintf(buffer, "%d", (int)const_float_value);
              break;

            case GreatThanOP:
              const_float_value = oprand01->pdata->var_pdata.fVal >
                                  oprand02->pdata->var_pdata.fVal;
              sprintf(buffer, "%d", (int)const_float_value);
              break;

            case LessThanOP:
              const_float_value = oprand01->pdata->var_pdata.fVal <
                                  oprand02->pdata->var_pdata.fVal;
              sprintf(buffer, "%d", (int)const_float_value);
              break;

            case GreatEqualOP:
              const_float_value = oprand01->pdata->var_pdata.fVal >=
                                  oprand02->pdata->var_pdata.fVal;
              sprintf(buffer, "%d", (int)const_float_value);
              break;

            case LessEqualOP:
              const_float_value = oprand01->pdata->var_pdata.fVal <=
                                  oprand02->pdata->var_pdata.fVal;
              sprintf(buffer, "%d", (int)const_float_value);
              break;
            default:
              break;
            }
            if (HashMapContain(constant_single_value_hashmap, buffer))
              cur = HashMapGet(constant_single_value_hashmap, buffer);
            else {
              cur = (Value *)malloc(sizeof(Value));
              value_init(cur);
              cur->VTy->TID = ImmediateFloatTyID;
              cur->name = strdup(buffer);
              cur->pdata->var_pdata.iVal = (int)const_float_value;
              cur->pdata->var_pdata.fVal = const_float_value;
              HashMapPut(constant_single_value_hashmap, strdup(buffer), cur);
            }
          } else {
            switch (((Instruction *)element)->opcode) {
            case AddOP:
              const_int_value = oprand01->pdata->var_pdata.iVal +
                                oprand02->pdata->var_pdata.iVal;
              sprintf(buffer, "%d", const_int_value);
              break;
            case SubOP:
              const_int_value = oprand01->pdata->var_pdata.iVal -
                                oprand02->pdata->var_pdata.iVal;
              sprintf(buffer, "%d", const_int_value);
              break;
            case MulOP:
              const_int_value = oprand01->pdata->var_pdata.iVal *
                                oprand02->pdata->var_pdata.iVal;
              sprintf(buffer, "%d", const_int_value);
              break;
            case DivOP:
              const_int_value = oprand01->pdata->var_pdata.iVal /
                                oprand02->pdata->var_pdata.iVal;
              sprintf(buffer, "%d", const_int_value);
              break;

            case EqualOP:
              const_int_value = oprand01->pdata->var_pdata.iVal ==
                                oprand02->pdata->var_pdata.iVal;
              sprintf(buffer, "%d", (int)const_int_value);
              break;

            case NotEqualOP:
              const_int_value = oprand01->pdata->var_pdata.iVal !=
                                oprand02->pdata->var_pdata.iVal;
              sprintf(buffer, "%d", (int)const_int_value);
              break;

            case GreatThanOP:
              const_int_value = oprand01->pdata->var_pdata.iVal >
                                oprand02->pdata->var_pdata.iVal;
              sprintf(buffer, "%d", (int)const_int_value);
              break;

            case LessThanOP:
              const_int_value = oprand01->pdata->var_pdata.iVal <
                                oprand02->pdata->var_pdata.iVal;
              sprintf(buffer, "%d", (int)const_int_value);
              break;

            case GreatEqualOP:
              const_int_value = oprand01->pdata->var_pdata.iVal >=
                                oprand02->pdata->var_pdata.iVal;
              sprintf(buffer, "%d", (int)const_int_value);
              break;

            case LessEqualOP:
              const_int_value = oprand01->pdata->var_pdata.iVal <=
                                oprand02->pdata->var_pdata.iVal;
              sprintf(buffer, "%d", (int)const_int_value);
              break;
            default:
              break;
            }
            if (HashMapContain(constant_single_value_hashmap, buffer))
              cur = HashMapGet(constant_single_value_hashmap, buffer);
            else {
              cur = (Value *)malloc(sizeof(Value));
              value_init(cur);
              cur->VTy->TID = ImmediateIntTyID;
              cur->name = strdup(buffer);
              cur->pdata->var_pdata.iVal = const_int_value;
              cur->pdata->var_pdata.fVal = (float)const_int_value;
              HashMapPut(constant_single_value_hashmap, strdup(buffer), cur);
            }
          }
          replace_use_other_by_self(cur, (Value *)element, cur_handle_bblock);
          delete_ins(cur_handle_list, &iter, &element);
        } else {
          iter_next_ins(&iter, &i, &element);
        }
      } else if (element->user.num_oprands == 1 &&
                 element->opcode == AssignOP) {
        Value *oprand01 = user_get_operand_use((User *)element, 0)->Val;
        if (oprand01->VTy->TID == ImmediateIntTyID ||
            oprand01->VTy->TID == ImmediateFloatTyID) {
          Value *cur = NULL;
          char buffer[30];
          float const_value = 0.f;
          if (((Value *)element)->VTy->TID == FloatTyID) {
            const_value = oprand01->pdata->var_pdata.iVal;
            sprintf(buffer, "%f", const_value);
          } else {
            const_value = oprand01->pdata->var_pdata.fVal;
            sprintf(buffer, "%d", (int)const_value);
          }
          if (HashMapContain(constant_single_value_hashmap, buffer))
            cur = HashMapGet(constant_single_value_hashmap, buffer);
          else {
            cur = (Value *)malloc(sizeof(Value));
            value_init(cur);
            cur->VTy->TID = ((Value *)element)->VTy->TID + 4;
            cur->name = strdup(buffer);
            cur->pdata->var_pdata.iVal = const_value;
            cur->pdata->var_pdata.fVal = const_value;
            HashMapPut(constant_single_value_hashmap, strdup(buffer), cur);
          }
          replace_use_other_by_self(cur, (Value *)element, cur_handle_bblock);
          delete_ins(cur_handle_list, &iter, &element);
        } else {
          iter_next_ins(&iter, &i, &element);
        }
      } else {
        iter_next_ins(&iter, &i, &element);
      }
    }
  }
}

void array_replace_optimization(Function *handle_func) {
  ALGraph *self_cfg = handle_func->self_cfg;

  // List *entry_block_list = (self_cfg->node_set)[0]->bblock_head->inst_list;

  for (int ii = 0; ii < self_cfg->node_num; ii++) {
    BasicBlock *cur_handle_bblock = (self_cfg->node_set)[ii]->bblock_head;
    List *cur_handle_list = (self_cfg->node_set)[ii]->bblock_head->inst_list;
    ListSetClean(cur_handle_list, CommonCleanInstruction);
    Instruction *element = NULL;
    int i = 0;
    ListNode *iter = cur_handle_list->data->head_;
    element = iter->element_;

    HashMap *array_replace = NULL;
    hashmap_init(&array_replace);

    while (i < ListSize(cur_handle_list)) {
      if (element->opcode == GetelementptrOP &&

          user_get_operand_use((User *)element, 0)
                  ->Val->pdata->array_pdata.top_array ==
              user_get_operand_use((User *)element, 0)->Val &&

          user_get_operand_use((User *)element, 1)->Val->VTy->TID ==
              ImmediateIntTyID) {
        Value *cur_handle_array = user_get_operand_use((User *)element, 0)->Val;
        HashMap *cur_replace = NULL;
        if (HashMapContain(array_replace, cur_handle_array->name)) {
          cur_replace = HashMapGet(array_replace, cur_handle_array->name);
        } else {
          hashmap_init(&cur_replace);
          HashMapPut(array_replace, strdup(cur_handle_array->name),
                     cur_replace);
        }

        Value *temp = user_get_operand_use((User *)element, 1)->Val;
        int total_offset = temp->pdata->var_pdata.iVal *
                           cur_handle_array->pdata->array_pdata.step_long;
        Value *top_value = cur_handle_array;

        if (ListSize(((Value *)element)->pdata->array_pdata.list_para) != 0) {
          iter_next_ins(&iter, &i, &element);
          while (i < ListSize(cur_handle_list)) {
            if (element->opcode == GetelementptrOP &&

                user_get_operand_use((User *)element, 0)
                        ->Val->pdata->array_pdata.top_array == top_value &&

                user_get_operand_use((User *)element, 1)->Val->VTy->TID ==
                    ImmediateIntTyID) {
              Value *cur_handle_array =
                  user_get_operand_use((User *)element, 0)->Val;
              Value *temp = user_get_operand_use((User *)element, 1)->Val;
              total_offset += temp->pdata->var_pdata.iVal *
                              cur_handle_array->pdata->array_pdata.step_long;
              if (ListSize(((Value *)element)->pdata->array_pdata.list_para) ==
                  0) {
                char intptr_buffer[50];
                sprintf(intptr_buffer, "%d", total_offset);
                iter_next_ins(&iter, &i, &element);
                if (element->opcode == StoreOP) {
                  HashMapPut(cur_replace, strdup(intptr_buffer),
                             user_get_operand_use((void *)element, 1)->Val);
                } else if (element->opcode == LoadOP) {
                  Value *val = NULL;
                  if (HashMapContain(cur_replace, intptr_buffer)) {
                    val = HashMapGet(cur_replace, intptr_buffer);
                    replace_use_other_by_self((Value *)val, (Value *)element,
                                              cur_handle_bblock);
                    delete_ins(cur_handle_list, &iter, &element);
                  }
                } else {
                  assert(0 && "replace array ");
                }
                break;
              }
              iter_next_ins(&iter, &i, &element);
            } else
              break;
          }
        } else {
          char intptr_buffer[50];
          sprintf(intptr_buffer, "%d", total_offset);
          iter_next_ins(&iter, &i, &element);
          if (element->opcode == StoreOP) {
            HashMapPut(cur_replace, strdup(intptr_buffer),
                       user_get_operand_use((void *)element, 1)->Val);
            iter_next_ins(&iter, &i, &element);
          } else if (element->opcode == LoadOP) {
            Value *val = NULL;
            if (HashMapContain(cur_replace, intptr_buffer)) {
              val = HashMapGet(cur_replace, intptr_buffer);
              replace_use_other_by_self((Value *)val, (Value *)element,
                                        cur_handle_bblock);
              delete_ins(cur_handle_list, &iter, &element);
            }
          } else {
            assert(0 && "replace array ");
          }
        }
      } else {
        iter_next_ins(&iter, &i, &element);
      }
    }
  }
}

void delete_non_used_block(Function *handle_func) {
  ALGraph *self_cfg = handle_func->self_cfg;

  // List *entry_block_list = (self_cfg->node_set)[0]->bblock_head->inst_list;

  for (int ii = 0; ii < self_cfg->node_num; ii++) {
    List *cur_handle_list = (self_cfg->node_set)[ii]->bblock_head->inst_list;
    ListSetClean(cur_handle_list, CommonCleanInstruction);
    Instruction *element;
    int i = 0;
    ListNode *iter = cur_handle_list->data->head_;
    element = iter->element_;

    while (i < ListSize(cur_handle_list)) {
      if (element->opcode == LabelOP &&
          iter_next_ins(&iter, &i, &element)->opcode == GotoOP) {
        delete_ins(cur_handle_list, &iter, &element);
      } else
        iter_next_ins(&iter, &i, &element);
    }
  }
}

void gcm(Function *handle_func) {
  ALGraph *self_cfg = handle_func->self_cfg;
  HashMap *ins_subordinate_block;
  hashmap_init(&ins_subordinate_block);
  HeadNode *root_headnode = (self_cfg->node_set)[0];

  for (int ii = 0; ii < self_cfg->node_num; ii++) {
    HeadNode *cur_handle_headnode = (self_cfg->node_set)[ii];
    List *cur_handle_list = cur_handle_headnode->bblock_head->inst_list;
    ListFirst(cur_handle_list, false);
    Instruction *element;
    while (ListNext(cur_handle_list, (void **)&element))
      element->ins_subordinate_block = cur_handle_headnode;
  }

  for (int ii = 0; ii < self_cfg->node_num; ii++) {
    List *cur_handle_list = (self_cfg->node_set)[ii]->bblock_head->inst_list;
    ListSetClean(cur_handle_list, CommonCleanInstruction);
    Instruction *element;
    int i = 0;
    ListNode *iter = cur_handle_list->data->head_;
    element = iter->element_;

    while (i < ListSize(cur_handle_list)) {
      for (int ii = 0; ii < self_cfg->node_num; ii++) {
        List *cur_handle_list =
            (self_cfg->node_set)[ii]->bblock_head->inst_list;
        ListSetClean(cur_handle_list, CommonCleanInstruction);
        Instruction *element;
        int i = 0;
        ListNode *iter = cur_handle_list->data->head_;
        element = iter->element_;

        while (i < ListSize(cur_handle_list)) {
        }
      }
    }
  }
}

#define N_OP_NUM 12
static char *NORMAL_OPERATOR[N_OP_NUM] = {
    "ERROROP", "PLUS",     "MINUS", "MOD",  "STAR",       "DIV",
    "EQUAL",   "NOTEQUAL", "GREAT", "LESS", "GREATEQUAL", "LESSEQUAL",
};

#define L_OP_NUM 2
static char *LOGIC_OPERATOR[L_OP_NUM] = {"AND", "OR"};

typedef struct _public_pair {
  TAC_OP opcode;
  Value *oprand1;
  Value *oprand2;
  Value *replace;
} public_pair;

unsigned HashKeyAddressPublicPair(public_pair *key) {
  return HashDjb2(key->oprand1->name) + HashDjb2(key->oprand2->name);
}

int ComparePublic(public_pair *lhs, public_pair *rhs) {
  return !(lhs->opcode == rhs->opcode &&
           ((lhs->oprand1 == rhs->oprand1 && lhs->oprand2 == rhs->oprand2) ||
            (lhs->oprand2 == rhs->oprand1 && lhs->oprand1 == rhs->oprand2)));
}

void hashmap_init_public_pair(HashMap **self) {
  *self = HashMapInit();
  HashMapSetHash(*self, (unsigned (*)(void *))HashKeyAddressPublicPair);
  HashMapSetCompare(*self, (int (*)(void *, void *))ComparePublic);
  HashMapSetCleanKey(*self, CleanHashSetKey);
  HashMapSetCleanValue(*self, CleanHashSetKey);
}

void public_expression_substitution_opt(Function *handle_func) {
  ALGraph *self_cfg = handle_func->self_cfg;

  // List *entry_block_list = (self_cfg->node_set)[0]->bblock_head->inst_list;
  HashMap *public_expression_set;
  hashmap_init_public_pair(&public_expression_set);

  for (int ii = 0; ii < self_cfg->node_num; ii++) {
    BasicBlock *cur_handle_bblock = (self_cfg->node_set)[ii]->bblock_head;
    List *cur_handle_list = (self_cfg->node_set)[ii]->bblock_head->inst_list;
    ListSetClean(cur_handle_list, CommonCleanInstruction);
    Instruction *element;
    int i = 0;
    ListNode *iter = cur_handle_list->data->head_;
    element = iter->element_;

    while (i < ListSize(cur_handle_list)) {
      if (((User *)element)->num_oprands == 2 &&
          element->opcode != GetelementptrOP && element->opcode != PhiFuncOp) {
        Value *oprand1 = user_get_operand_use((User *)element, 0)->Val;
        Value *oprand2 = user_get_operand_use((User *)element, 1)->Val;
        public_pair *fuckk = malloc(sizeof(public_pair));
        fuckk->oprand1 = oprand1;
        fuckk->oprand2 = oprand2;
        fuckk->opcode = element->opcode;

        if (HashMapContain(public_expression_set, fuckk)) {
          public_pair *findd = NULL;
          findd = HashMapGet(public_expression_set, fuckk);
          replace_use_other_by_self(findd->replace, (Value *)element,
                                    cur_handle_bblock);
          free(fuckk);
          delete_ins(cur_handle_list, &iter, &element);
        } else {
          fuckk->replace = (Value *)element;
          HashMapPut(public_expression_set, fuckk, fuckk);
          iter_next_ins(&iter, &i, &element);
        }
      } else {
        iter_next_ins(&iter, &i, &element);
      }
    }
  }
}