#ifndef VALUE_DEF
#define VALUE_DEF

#include <stdbool.h>
#include <stdlib.h>
// #include <stddef.h>
#include <stdint.h>

#include "cds.h"
#include "type.h"

struct _Symtab;
struct _Use;

#define NumUserOperandsBits 27

typedef struct _Value Value;

typedef union _PData {
  // 跳转的目的地 跳转的条件放在use里
  struct {
    Value *goto_location;  // 无条件跳转位置
  } no_condition_goto;

  struct {
    Value *true_goto_location;   // 条件为真跳转位置
    Value *false_goto_location;  // 条件为假跳转位置
  } condition_goto;

  // 常数字面量
  struct {
    union {
      int iVal;
      float fVal;
    };
  } var_pdata;

  struct {
    TypeID return_type;  // 返回类型
    // Type param_type_lists[10];  // 参数的类型数组
    int param_num;  // 传入参数的个数

  } symtab_func_pdata;  // 目前只在我的符号表里用的结构，最终func结构还未完全确定

  struct {
    Value *return_value;  // 返回值
  } return_pdata;

  struct {
    Value *param_value;  // 函数参数
  } param_pdata;

  struct {
    Value *point_value;  // 分配内存的变量
  } allocate_pdata;

  struct {
    HashMap *phi_value;
  } phi_func_pdata;

} PData;

struct _Value {
  Type *VTy;

  struct _Use *use_list;

  PData *pdata;

  char *name;

  unsigned char HasValueHandle : 1;  // Has a ValueHandle pointing to this?

  unsigned NumUserOperands : NumUserOperandsBits;  // 用于指示被多少个user调用

  // Use the same type as the bitfield above so that MSVC will pack them.
  unsigned IsUsedByMD : 1;
  unsigned HasName : 1;
  unsigned HasMetadata : 1;     // Has metadata attached to this?
  unsigned HasHungOffUses : 1;  // 用于指示有多少个操作数
};

void value_init(Value *this);

void value_free(Value *this);

void value_init_int(Value *this, TypeID type, int num);

void value_copy(Value *this, Value *copy);

Value *value_init_int_with_initial(int num);

Value *value_init_float_with_initial(float num);

void *get_pdata(Value *this);

// zzq
Value *value_init_const_int(int num);

// zzq
Value *value_init_const_float(float num);

void value_add_use(Value *this, struct _Use *U);

Type *getType(Value *this);

void value_set_name(Value *this, char *name);

#endif