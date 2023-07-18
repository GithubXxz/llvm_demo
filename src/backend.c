#include "Pass.h"
#include "cds.h"
#include "function.h"

static const int REGISTER_NUM = 3;
typedef enum _LOCATION { R1 = 1, R2, R3, MEMORY } LOCATION;
static char *location_string[] = {"null", "R1", "R2", "R3", "M"};

void register_replace(Function *handle_func) {
  HashMap *var_location = handle_func->var_localtion;
  ALGraph *self_cfg = handle_func->self_cfg;

  Pair *ptr_pair;
  HashMapFirst(var_location);
  while ((ptr_pair = HashMapNext(var_location)) != NULL) {
    printf("\tvar:%s\taddress:%s\n ", (char *)ptr_pair->key,
           location_string[*((LOCATION *)ptr_pair->value)]);
  }
}
