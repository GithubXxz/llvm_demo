#include "instruction.h"

#include "stdio.h"
#include "type.h"

// TODO 应该从上级获得两个操作数的信息来更新User中value的type?
Instruction *ins_new(int op_num, Value *self) {
  // 计算use的大小
  // 二元运算就是两个use所需要的内存空间拼接到Instruction中构成Instruction的use链表
  int user_size = user_get_size(op_num);
  int use_size = user_size - sizeof(User);
  // use 是额外的，所以要单独计算大小
  // 为了 instruction 分配内存块 并且返回内存块的首地址
  uint8_t *storage = (uint8_t *)malloc(sizeof(Instruction) + use_size);
  // 不考虑SSA形式下的instruction
  ((User *)(storage + use_size))->res = self;
  // 将instruction里面的内容分布在内存块中
  user_construct(storage, op_num);
  // 计算偏移量然后返回User的首地址 低地址存储的位Use
  return (Instruction *)(storage + use_size);
}

Instruction *ins_new_no_operator(Value *self, TAC_OP Op) {
  Instruction *inst = ins_new(0, self);
  inst->opcode = Op;
  return inst;
}

Instruction *ins_new_single_operator(Value *self, TAC_OP Op, Value *S1) {
  Instruction *inst = ins_new(1, self);
  inst->opcode = Op;
  Use *puse = user_get_operand_use((User *)inst, 0);
  value_add_use(S1, puse);
  return inst;
}

Instruction *ins_new_binary_operator(Value *self, TAC_OP Op, Value *S1,
                                     Value *S2) {
  Instruction *inst = ins_new(2, self);
  inst->opcode = Op;
  Use *puse = user_get_operand_use((User *)inst, 0);
  value_add_use(S1, puse);
  puse = user_get_operand_use((User *)inst, 1);
  value_add_use(S2, puse);
  return inst;
}

TypeID imm_res_type(Value *left, Value *right) {
  if (left->VTy->TID == ImmediateIntTyID &&
      right->VTy->TID == ImmediateIntTyID) {
    return ImmediateIntTyID;
  } else {
    return ImmediateFloatTyID;
  }
}

TypeID ins_res_type(Value *left, Value *right) {
  TypeID type_id = FloatTyID;
  TypeID s1_type = left->VTy->TID;
  TypeID s2_type = right->VTy->TID;

  if ((s1_type == IntegerTyID || s1_type == ImmediateIntTyID) &&
      (s2_type == IntegerTyID || s2_type == ImmediateIntTyID)) {
    type_id = IntegerTyID;
  }
  return type_id;
}

void free_common_ins(Instruction *self) {
  // 释放连接这条instruction的use链
  for (int i = 0; i < self->user.num_oprands; i++) {
    use_remove_from_list(user_get_operand_use((User *)self, i));
  }
  value_free((Value *)self);
  free(user_get_operand_use((User *)self, 0));
}

// clean the instruction
void CommonCleanInstruction(void *element) {
  free_common_ins((Instruction *)element);
  element = NULL;
}

/*
SSA版本
非Value*版本
*/

Instruction *ins_new_v2(int op_num) {
  // 计算use的大小
  // 二元运算就是两个use所需要的内存空间拼接到Instruction中构成Instruction的use链表
  int user_size = user_get_size(op_num);
  int use_size = user_size - sizeof(User);
  // use 是额外的，所以要单独计算大小
  // 为了 instruction 分配内存块 并且返回内存块的首地址
  uint8_t *storage = (uint8_t *)malloc(sizeof(Instruction) + use_size);
  if (storage == NULL) {
    printf("no no no no no!!!!!!\n");
  }
  value_init((Value *)(storage + use_size));
  // 将instruction里面的内容分布在内存块中
  user_construct(storage, op_num);
  // 计算偏移量然后返回User的首地址 低地址存储的位Use
  return (Instruction *)(storage + use_size);
}

Instruction *ins_new_phi_func_v2(TAC_OP Op, int num_of_predecessor) {
  Instruction *inst = ins_new_v2(num_of_predecessor);
  ((Value *)inst)->HasHungOffUses = true;
  inst->opcode = Op;
  return inst;
}

Instruction *ins_new_no_operator_v2(TAC_OP Op) {
  Instruction *inst = ins_new_v2(0);
  inst->opcode = Op;
  return inst;
}

Instruction *ins_new_single_operator_v2(TAC_OP Op, Value *S1) {
  Instruction *inst = ins_new_v2(1);
  inst->opcode = Op;
  Use *puse = user_get_operand_use((User *)inst, 0);
  value_add_use(S1, puse);
  return inst;
}

Instruction *ins_new_binary_operator_v2(TAC_OP Op, Value *S1, Value *S2) {
  Instruction *inst = ins_new_v2(2);
  inst->opcode = Op;
  Use *puse = user_get_operand_use((User *)inst, 0);
  value_add_use(S1, puse);
  puse = user_get_operand_use((User *)inst, 1);
  value_add_use(S2, puse);
  return inst;
}

void phi_node_copy(TAC_OP Op, Value *res, Value *S1) {}

TypeID ins_res_type_v2(Value *left, Value *right) {
  TypeID type_id = VoidTyID;
  TypeID s1_type = left->VTy->TID;
  TypeID s2_type = right->VTy->TID;
  if ((s1_type == IntegerTyID || s1_type == ImmediateIntTyID) &&
      (s2_type == IntegerTyID || s2_type == ImmediateIntTyID)) {
    type_id = IntegerTyID;
  }

  if ((s1_type == FloatTyID || s1_type == ImmediateFloatTyID) &&
      (s2_type == FloatTyID || s2_type == ImmediateFloatTyID)) {
    type_id = FloatTyID;
  }
  return type_id;
}

void free_common_ins_v2(Instruction *self) {
  if (self->opcode == AddOP || self->opcode == SubOP || self->opcode == MulOP ||
      self->opcode == DivOP || self->opcode == EqualOP ||
      self->opcode == ModOP || self->opcode == GreatThanOP ||
      self->opcode == LessThanOP || self->opcode == ParamOP ||
      self->opcode == NotEqualOP || self->opcode == GreatEqualOP ||
      self->opcode == LessEqualOP) {
    // 释放连接这条instruction的use链
    for (int i = 0; i < self->user.num_oprands; i++) {
      use_remove_from_list(user_get_operand_use((User *)self, i));
      free(user_get_operand_use((User *)self, i));
    }
    free(self);
  } else if (self->opcode == GotoOP || self->opcode == GotoWithConditionOP ||
             self->opcode == CallOP || self->opcode == ReturnOP) {
    // 释放连接这条instruction的use链
    for (int i = 0; i < self->user.num_oprands; i++) {
      use_remove_from_list(user_get_operand_use((User *)self, i));
      free(user_get_operand_use((User *)self, i));
    }
    value_free(((User *)self)->res);
    free(self);
  }
}

// clean the instruction
void CommonCleanInstruction_v2(void *element) {
  free_common_ins((Instruction *)element);
  element = NULL;
}

void use_relation_test() {
  printf("this is the user and use test\n");
  List *ins_list = ListInit();
  ListSetClean(ins_list, CleanObject);

  Value *temp1 = (Value *)malloc(sizeof(Value));
  value_init(temp1);
  temp1->VTy->TID = IntegerTyID;
  temp1->name = strdup("temp1");

  Value *temp2 = (Value *)malloc(sizeof(Value));
  value_init(temp2);
  temp2->VTy->TID = IntegerTyID;
  temp2->name = strdup("temp2");

  // 创建指针
  Value *temp3 = (Value *)ins_new_single_operator_v2(AddOP, temp1);
  // 添加变量类型
  temp3->VTy->TID = IntegerTyID;
  // 添加指针的名字 映射进哈希表 放入symbol_tabel里面 用于索引
  temp3->name = strdup("temp3");

  // 创建指针
  Value *temp4 = (Value *)ins_new_single_operator_v2(AddOP, temp1);
  // 添加变量类型
  temp4->VTy->TID = IntegerTyID;
  // 添加指针的名字 映射进哈希表 放入symbol_tabel里面 用于索引
  temp4->name = strdup("temp4");

  // 创建指针
  Value *temp5 = (Value *)ins_new_single_operator_v2(AddOP, temp2);
  // 添加变量类型
  temp5->VTy->TID = IntegerTyID;
  // 添加指针的名字 映射进哈希表 放入symbol_tabel里面 用于索引
  temp5->name = strdup("temp5");

  printf("this %s used by ", temp1->name);
  printf("%s,", ((Value *)temp1->use_list->Parent)->name);
  printf("%s\n", ((Value *)temp1->use_list->Next->Parent)->name);

  printf("this %s use ", temp3->name);
  printf("%s,", user_get_operand_use(((User *)temp3), 0)->Val->name);
  printf("\n");

  if (temp1->use_list != NULL) {
    Use *u1 = (Use *)(temp1->use_list);
    Use *u2 = u1->Next;
    while (u1 != NULL) {
      value_add_use(temp2, u1);
      u1 = u2;
      u2 = (u2 == NULL ? NULL : u2->Next);
    }
  }

  temp1->use_list = NULL;

  printf("this %s used by ", temp2->name);
  printf("%s,", ((Value *)temp2->use_list->Parent)->name);
  printf(" %s,", ((Value *)temp2->use_list->Next->Parent)->name);
  printf(" %s\n", ((Value *)temp2->use_list->Next->Next->Parent)->name);

  printf("this %s use ", temp3->name);
  printf("%s,", user_get_operand_use(((User *)temp3), 0)->Val->name);
  printf("\n");

  ListPushBack(ins_list, temp2);
}

// zzq 判断一个value是否满足pdata里面的数据为0
// bool is_zero(Value *this) {
//   return (this->pdata != NULL && *((int *)(this->pdata)) == 0);
// }

// zzq
// Instruction *ins_new_binary_operator(char Op, Value *S1, Value *S2) {
//   TypeID type_id = VoidTyID;
//   void *pdata = NULL;

//   if ((S1->VTy->TID == IntegerTyID || S1->VTy->TID == ConstIntTyID) &&
//       (S2->VTy->TID == IntegerTyID || S2->VTy->TID == ConstIntTyID)) {
//     type_id = IntegerTyID;

//     if (is_zero(S1)) {
//       switch (Op) {
//         case '+':
//           return ins_new_one_operator(S2);
//         case '-':
//           return ins_new_one_operator(value_init_int_with_initial(
//               *((int *)S1->pdata) - *((int *)S2->pdata)));
//         case '*':
//           return ins_new_one_operator(value_init_int_with_initial(
//               *((int *)S1->pdata) * *((int *)S2->pdata)));
//         case '/':
//           return ins_new_one_operator(value_init_int_with_initial(
//               *((int *)S1->pdata) / *((int *)S2->pdata)));
//         default:
//           printf("非法运算符\n");
//       }
//     }

//     // 常数折叠 如果两个操作数的字面量都是常数
//     //则在构造的时候自动折叠为一个常数 if (S1->pdata != NULL && S2->pdata
//     != NULL) {
//       switch (Op) {
//         case '+':
//           return ins_new_one_operator(value_init_int_with_initial(
//               *((int *)S1->pdata) + *((int *)S2->pdata)));
//         case '-':
//           return ins_new_one_operator(value_init_int_with_initial(
//               *((int *)S1->pdata) - *((int *)S2->pdata)));
//         case '*':
//           return ins_new_one_operator(value_init_int_with_initial(
//               *((int *)S1->pdata) * *((int *)S2->pdata)));
//         case '/':
//           return ins_new_one_operator(value_init_int_with_initial(
//               *((int *)S1->pdata) / *((int *)S2->pdata)));
//         default:
//           printf("非法运算符\n");
//       }
//     }
//   } else if ((S1->VTy->TID == FloatTyID || S1->VTy->TID == ConstFloatTyID)
//   &&
//              (S2->VTy->TID == FloatTyID || S2->VTy->TID == ConstFloatTyID))
//              {
//     type_id = FloatTyID;

//     // 如果有操作数为0则去掉
//     if ((S1->pdata != NULL && *((float *)(S1->pdata)) == 0.0f)) {
//       return ins_new_one_operator(S2);
//     }

//     // 如果有操作数为0则去掉
//     if ((S2->pdata != NULL && *((float *)(S2->pdata)) == 0.0f)) {
//       return ins_new_one_operator(S1);
//     }

//     if (S1->pdata != NULL && S2->pdata != NULL) {
//       switch (Op) {
//         case '+':
//           return ins_new_one_operator(value_init_float_with_initial(
//               *((float *)S1->pdata) + *((float *)S2->pdata)));
//         case '-':
//           return ins_new_one_operator(value_init_float_with_initial(
//               *((float *)S1->pdata) - *((float *)S2->pdata)));
//         case '*':
//           return ins_new_one_operator(value_init_float_with_initial(
//               *((float *)S1->pdata) * *((float *)S2->pdata)));
//         case '/':
//           return ins_new_one_operator(value_init_float_with_initial(
//               *((float *)S1->pdata) / *((float *)S2->pdata)));
//         default:
//           printf("非法运算符\n");
//       }
//     }
//   }

//   Instruction *inst = ins_new(2, type_id, pdata);
//   // 将 inst 这个 user 加入到 s1 和 s2 这两个 value 的 use_list

//   // 测试是否将数据相加放入Instruction里面的User里的value
//   //    printf("%d\n", *((int *) (((Value *) inst)->pdata)));

//   //
//   //
//   测试是否将数据相加放入Instruction里面的User里的userlist指向的第一个use的地址的父亲是否指向本身
//   //    printf("%d\n", *((int *)
//   //    (user_get_value(use_get_parent(inst->user.use_list)))->pdata));

//   Use *puse = user_get_operand_use((User *)inst, 0);
//   value_add_use(S1, puse);

//   puse = user_get_operand_use(&inst->user, 1);
//   value_add_use(S2, puse);

//   return inst;
// }

// Instruction *ins_new_one_operator(Value *S1) {
//   TypeID type_id = S1->VTy->TID;
//   void *pdata = S1->pdata;
//   Instruction *inst = ins_new(1, type_id, pdata);
//   // 将 inst 这个 user 加入到 s1 和 s2 这两个 value 的 use_list
//   // 测试是否将数据相加放入Instruction里面的User里的value
//   printf("%f\n", *((float *)(((Value *)inst)->pdata)));
//   //
//   测试是否将数据相加放入Instruction里面的User里的userlist指向的第一个use的地址的父亲是否指向本身
//   printf("%f\n", *((float *)(user_get_value(use_get_parent(
//                                  user_get_operand_use(&(inst->user), 0))))
//                        ->pdata));
//   Use *puse = user_get_operand_use(&(inst->user), 0);
//   value_add_use(S1, puse);
//   return inst;
// }

Value *ins_get_value(Instruction *this) { return (Value *)this; }
