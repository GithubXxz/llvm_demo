
#include "function.h"

void function_init(Function *this) {
  this->label = NULL;
  this->entry_bblock = NULL;
  this->end_bblock = NULL;
  this->num_of_block = 0;
  this->all_var_live_interval = ListInit();
  this->self_cfg = NULL;
  HashMap *var_location = HashMapInit();
  hashmap_init(&var_location);
  this->var_localtion = var_location;
}