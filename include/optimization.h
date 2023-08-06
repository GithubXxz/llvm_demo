#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "Pass.h"
void calculate_live_use_def_by_graph(ALGraph *self);
void calculate_live_in_out(ALGraph *self);
void calculate_live_interval(ALGraph *self_cfg, Function *self_func);
void line_scan_register_allocation(Function *self_func);

void delete_non_used_var_pass(Function *self);
void array_replace_optimization(Function *handle_func);

#endif