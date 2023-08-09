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

#define NumUserOperandsBits 15

typedef struct _Value Value;

typedef union _PData PData;

struct _Value {
  Type *VTy;

  struct _Use *use_list;

  PData *pdata;

  char *name;

  unsigned char HasValueHandle : 1; // Has a ValueHandle pointing to this?

  unsigned NumUserOperands : NumUserOperandsBits; // 用于指示被多少个user调用

  // Use the same type as the bitfield above so that MSVC will pack them.
  unsigned IsUsedByMD : 1;
  unsigned HasName : 1;
  unsigned IsInitArgs : 1;     // is cur ins the func param init?
  unsigned HasHungOffUses : 1; // 用于指示有多少个操作数
  unsigned IsGlobalVar : 1;
  unsigned IsConst : 1;
};

union _PData {
  // 跳转的目的地 跳转的条件放在use里
  struct {
    Value *goto_location; // 无条件跳转位置
  } no_condition_goto;

  struct {
    Value *true_goto_location;  // 条件为真跳转位置
    Value *false_goto_location; // 条件为假跳转位置
  } condition_goto;

  // 常数字面量
  struct {
    int iVal;
    float fVal;
  } var_pdata;

  struct {
    TypeID return_type;       // 返回类型
    TypeID *param_type_lists; // 参数的类型数组
    int param_num;            // 传入参数的个数
  } symtab_func_pdata;

  struct {
    TypeID return_type; // 返回类型
  } return_pdata;

  struct {
    char *name;
  } func_call_pdata;

  struct {
    TypeID param_type; // 返回类型
  } param_pdata;

  struct {
    Value *point_value; // 分配内存的变量
  } allocate_pdata;

  struct {
    // phi函数对饮value的pdata 里面存有<block*,value*>的kv对
    HashMap *phi_value;
    Value *phi_pointer;
    int offset_var_use;
    // HashMap *phi_assign_choose;
    // int num_of_predecessor;
  } phi_func_pdata;

  struct {
    Value *phi_replace_value;
  } phi_replace_pdata;

  struct {
    int the_param_index;
  } param_init_pdata;

  struct {
    // 判断是否为最外级别数组
    Value *top_array;
    TypeID array_type;
    // 链表 各层数组的元素个数
    List *list_para;
    // 成员数量
    int total_member;
    // 步长
    int step_long;
    // 记录局部的数组的赋值和引用的信息
    // HashMap *local_array_hashmap;

    //用于判断数据是否为局部数组
    int is_local_array;
  } array_pdata;
};

// 用于全局数组初始化后 后端memory copy
typedef struct {
  int offset;
  float fval;
  int ival;
} global_array_init_item;

// 局部数组 且未被函数当作参数传递 的表

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
