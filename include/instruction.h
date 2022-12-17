#include "user.h"

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef enum _TAC_OP {
  AddOP,
  SubOP,
  MulOP,
  DivOP,
  GotoOP,
  GotoWithConditionOP,
  CallOP,
  FunBeginOP,
  FunEndOP,
  ReturnOP,
  AssignOP,
  CallWithReturnValueOP,
  NotEqualOP,
  EqualOP,
  GreatThanOP,
  LessThanOP,
  GreatEqualOP,
  LessEqualOP,
  LABELOP
} TAC_OP;

struct _BasicBlock;
typedef struct _Instruction {
  User user;
  TAC_OP opcode;  // 操作吗
  struct _BasicBlock *Parent;
  Value *res;
} Instruction;

Instruction *ins_new(int op_num, Value *self);

// 用于标识跳转
Instruction *ins_new_no_operator(Value *self, TAC_OP Op);

Instruction *ins_new_single_operator(Value *self, TAC_OP Op, Value *S1);

Instruction *ins_new_binary_operator(Value *self, TAC_OP Op, Value *S1,
                                     Value *S2);

void free_ins(Instruction *self);

Instruction *ins_new_one_operator(Value *S1);

TypeID ins_res_type(Value *left, Value *right);

Value *ins_get_value(Instruction *this);

// https://llvm.org/doxygen/InstrTypes_8h_source.html
/// Construct a binary instruction, given the opcode and the two
/// operands.  Optionally (if InstBefore is specified) insert the instruction
/// into a BasicBlock right before the specified instruction.  The specified
/// Instruction is allowed to be a dereferenced end iterator.
///
// static BinaryOperator *Create(BinaryOps Op, Value *S1, Value *S2,
//                                 const Twine &Name = Twine(),
//                                 Instruction *InsertBefore = nullptr);

//  BinaryOperator::BinaryOperator(BinaryOps iType, Value *S1, Value *S2,
//                                 Type *Ty, const Twine &Name,
//                                 BasicBlock *InsertAtEnd)
//    : Instruction(Ty, iType,
//                  OperandTraits<BinaryOperator>::op_begin(this),
//                  OperandTraits<BinaryOperator>::operands(this),
//                  InsertAtEnd) {
//    Op<0>() = S1;
//    Op<1>() = S2;
//    setName(Name);
//    AssertOK();
//  }

// template <int Idx, typename U> static Use &OpFrom(const U *that) {
//       return Idx < 0
//         ? OperandTraits<U>::op_end(const_cast<U*>(that))[Idx]
//         : OperandTraits<U>::op_begin(const_cast<U*>(that))[Idx];
//     }

//    template <int Idx> Use &Op() {
//      return OpFrom<Idx>(this);
//    }
//    template <int Idx> const Use &Op() const {
//      return OpFrom<Idx>(this);
//    }

// op_iterator       op_begin()       { return getOperandList(); }
//    const_op_iterator op_begin() const { return getOperandList(); }
//    op_iterator       op_end()         {
//      return getOperandList() + NumUserOperands;
//    }

#endif