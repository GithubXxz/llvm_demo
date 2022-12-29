#ifndef PASS
#define PASS

#include "bblock.h"
#include "c_container_auxiliary.h"
#include "cds.h"

void print_ins_pass(List *self);
void ins_toBBlock_pass(List *self);
void delete_return_deadcode_pass(List *self);
void print_bblock_pass(BasicBlock *self);

#endif
