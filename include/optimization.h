#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "Pass.h"
void calculate_live_use_def_by_graph(ALGraph *self);
void calculate_live_in_out(ALGraph *self);
void delete_non_live_var_pass();
void calculate_live_interval(ALGraph *self_cfg, Function *self_func);
void line_scan_register_allocation(Function *self_func);
#endif