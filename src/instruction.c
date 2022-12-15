#include "instruction.h"

#include "stdio.h"

// TODO 应该从上级获得两个操作数的信息来更新User中value的type?
Instruction *ins_new(int op_num, Value *self) {
  // 计算use的大小
  // 二元运算就是两个use所需要的内存空间拼接到Instruction中构成Instruction的use链表
  int user_size = user_get_size(op_num);
  int use_size = user_size - sizeof(User);
  // use 是额外的，所以要单独计算大小
  // 为了 instruction 分配内存块 并且返回内存块的首地址
  uint8_t *storage = (uint8_t *)malloc(sizeof(Instruction) + use_size);
  // 将instruction里面的内容分布在内存块中
  user_construct(storage, op_num);
  // 不考虑SSA形式下的instruction
  ((Instruction *)(storage + use_size))->res = self;
  // 计算偏移量然后返回User的首地址 低地址存储的位Use
  return (Instruction *)(storage + use_size);
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

TypeID ins_res_type(Value *left, Value *right) {
  TypeID type_id = DefaultTyID;
  TypeID s1_type = left->VTy->TID;
  TypeID s2_type = right->VTy->TID;
  if ((s1_type == IntegerTyID || s1_type == ConstIntTyID) &&
      (s2_type == IntegerTyID || s2_type == ConstIntTyID)) {
    type_id = IntegerTyID;
  }

  if ((s1_type == FloatTyID || s1_type == ConstFloatTyID) &&
      (s2_type == FloatTyID || s2_type == ConstFloatTyID)) {
    type_id = FloatTyID;
  }
}
// zzq 判断一个value是否满足pdata里面的数据为0
// bool is_zero(Value *this) {
//   return (this->pdata != NULL && *((int *)(this->pdata)) == 0);
// }

// zzq
// Instruction *ins_new_binary_operator(char Op, Value *S1, Value *S2) {
//   TypeID type_id = DefaultTyID;
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