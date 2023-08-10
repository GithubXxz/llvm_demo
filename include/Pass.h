#ifndef PASS
#define PASS

#include "bblock.h"
#include "c_container_auxiliary.h"
#include "cds.h"

// 使用邻接矩阵也可以做简单的替代，更简洁 二维数组也可以简单的替代
typedef struct _HeadNode {
  BasicBlock *bblock_head;
  // 邻接的节点集合边集
  HashMap *edge_list;
  // 他支配的节点集
  HashMap *dom_set;
  // 他的支配边界集合
  HashMap *dom_frontier_set;
  // 立即支配他的节点
  struct _HeadNode *idom_node;
  // 他的前驱节点链表
  List *pre_node_list;
  // 是否被访问过
  bool is_visited;
  int dom_depth;
} HeadNode; // 定义头结点

// 邻接表表示法的图
typedef struct _ALGraph {
  HeadNode **node_set;
  // 顶点数目和边的数目
  int node_num, edge_num;
} ALGraph; // 定义邻接表

void print_ins_pass(List *self);
void ins_toBBlock_pass(List *self);
void delete_return_deadcode_pass(List *self);
void print_bblock_pass(BasicBlock *self);
void bblock_to_dom_graph_pass(Function *self);
void print_bblock_pass_temp(BasicBlock *self);
void delete_alloca_store_load_ins_pass(ALGraph *self);
void printf_cur_func_ins(Function *self);

void replace_use_other_by_self(Value *self, Value *other,
                               BasicBlock *cur_handle_block);
void delete_ins(List *self, ListNode **iter, Instruction **element);
// inline void delete_ins(List *self, ListNode **iter, Instruction **element) {
//   ListNode *pred = (*iter)->pred_;
//   ListNode *succ = (*iter)->succ_;
//   pred->succ_ = succ;
//   succ->pred_ = pred;
//   self->data->func_clean_(*element);
//   free(*iter);
//   *iter = succ;
//   *element = (*iter)->element_;
//   self->data->size_--;
//   if (self->data->size_ == 0)
//     self->data->head_ = NULL;
// }

Instruction *iter_next_ins(ListNode **iter, int *i, Instruction **element);
// inline void iter_next_ins(ListNode **iter, int *i, Instruction **element) {
//   *iter = (*iter)->succ_;
//   (*i)++;
//   *element = (*iter)->element_;
// }

#endif
