#include "bblock.h"

void bblock_init(BasicBlock* this, Function* parent) {
  this->label = NULL;
  this->inst_list = ListInit();
  ListSetClean(this->inst_list, CleanObject);
  this->father_bblock_list = ListInit();
  ListSetClean(this->father_bblock_list, CleanObject);
  this->true_bblock = NULL;
  this->false_bblock = NULL;
  this->parent = parent;
  hashset_init_string(&(this->live_def));
  hashset_init_string(&(this->live_use));
  hashset_init_string(&(this->live_in));
  hashset_init_string(&(this->live_out));
}

// only used by bblock
// static InstNode* new_inst_node(Instruction* inst) {
// InstNode* n = malloc(sizeof(InstNode));
// n->inst = inst;
// // 初始化结点
// sc_list_init(&(n->list));
// return n;
// }

void bblock_add_inst_back(BasicBlock* this, Instruction* inst) {
  // InstNode* node = new_inst_node(inst);
  // sc_list_add_tail(&this->inst_list, &node->list);
}

Instruction* bblock_pop_inst_back(BasicBlock* this) {
  // struct sc_list *tmp = sc_list_pop_tail(&this->inst_list);
  // InstNode* pnode = sc_list_entry(tmp, InstNode, list);
  // return pnode->inst;
  return NULL;
}
