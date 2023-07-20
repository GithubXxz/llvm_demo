#include "Pass.h"
#include "cds.h"
#include "container/hash_map.h"
#include "function.h"

// static const int REGISTER_NUM = 3;
// typedef enum _LOCATION { R1 = 1, R2, R3, MEMORY } LOCATION;
// static char *location_string[] = {"null", "R1", "R2", "R3", "M"};

static const int REGISTER_NUM = 8;
char *location_string[] = {"null", "R4",  "R5",  "R6",  "R8",
                           "R9",   "R10", "R11", "R12", "M"};
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

extern HashMap *global_array_init_hashmap;

void register_replace(Function *handle_func) {
  HashMap *var_location = handle_func->var_localtion;
  ALGraph *self_cfg = handle_func->self_cfg;

#ifdef PRINT_OK
  Pair *ptr_pair;
  HashMapFirst(var_location);
  while ((ptr_pair = HashMapNext(var_location)) != NULL) {
    printf("\tvar:%s\taddress:%s\n ", (char *)ptr_pair->key,
           location_string[(LOCATION)(intptr_t)ptr_pair->value]);
  }
#endif
}
