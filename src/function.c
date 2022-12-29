
#include "function.h"

void function_init(Function* this) {
  this->label = NULL;
  this->entry_bblock = NULL;
  this->end_bblock = NULL;
}