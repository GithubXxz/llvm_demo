#include "value.h"

#include "stdio.h"
#include "string.h"
#include "type.h"
#include "use.h"

/// This method should only be used by the Use class.
// void addUse(Use &U) { U.addToList(&UseList); }
void value_add_use(Value *S1, Use *cur_use) {
  use_add_to_list(cur_use, &(S1->use_list));
  cur_use->Val = S1;
}

void value_init(Value *this) {
  // 初始化的作用全部置零
  // printf("this is between %8x and %8x\n", this, (char *)this +
  // sizeof(Value));
  memset(this, 0, sizeof(Value));
  this->VTy = (Type *)malloc(sizeof(Type));
  this->pdata = (PData *)malloc(sizeof(PData));
  this->VTy->NumContainedTys = 1;
  this->use_list = NULL;
  this->name = NULL;
}

void value_copy(Value *this, Value *copy) {
  // 拷贝类型
  *(this->VTy) = *(copy->VTy);
  // 拷贝内容
  *(this->pdata) = *(copy->pdata);
  // 无名
  this->HasName = 0;
}

void value_free(Value *this) {
  free(this->VTy);
  free(this->name);
  free(this->pdata);
}

Value *value_init_int_with_initial(int num) {
  Value *int_with_initial = (Value *)malloc(sizeof(Value));
  // 初始化的作用全部置零
  value_init(int_with_initial);
  int_with_initial->VTy->TID = IntegerTyID;
  int_with_initial->pdata = malloc(sizeof(int));
  *((int *)int_with_initial->pdata) = num;

  return int_with_initial;
}

Value *value_init_float_with_initial(float num) {
  Value *float_with_initial = (Value *)malloc(sizeof(Value));
  // 初始化的作用全部置零
  value_init(float_with_initial);
  float_with_initial->VTy->TID = FloatTyID;
  float_with_initial->pdata = malloc(sizeof(float));
  *((float *)float_with_initial->pdata) = num;

  return float_with_initial;
}

Value *value_init_int_without() {
  Value *int_with_initial = (Value *)malloc(sizeof(Value));
  // 初始化的作用全部置零
  value_init(int_with_initial);
  int_with_initial->VTy->TID = IntegerTyID;
  int_with_initial->pdata = malloc(sizeof(int));

  return int_with_initial;
}

Value *value_init_float_without() {
  Value *float_with_initial = (Value *)malloc(sizeof(Value));
  // 初始化的作用全部置零
  value_init(float_with_initial);
  float_with_initial->VTy->TID = FloatTyID;
  float_with_initial->pdata = malloc(sizeof(float));

  return float_with_initial;
}

// void value_init_float(Value *this, enum TypeID type, float num) {
//     //初始化的作用全部置零
//     value_init(this);
//     this->VTy->TID = type;
//     if (type == FloatTyID) {
//         this->pdata = malloc(sizeof(float));
//         *((float *) this->pdata) = num;
//     }
// }

// zzq 整数字面量
Value *value_init_const_int(int num) {
  // 初始化的作用全部置零
  Value *const_int = (Value *)malloc(sizeof(Value));
  value_init(const_int);
  const_int->VTy->TID = ImmediateIntTyID;
  const_int->pdata = malloc(sizeof(int));
  *((int *)const_int->pdata) = num;
  return const_int;
}

// zzq 浮点数字面量
Value *value_init_const_float(float num) {
  // 初始化的作用全部置零
  Value *const_float = (Value *)malloc(sizeof(Value));
  value_init(const_float);
  const_float->VTy->TID = ImmediateFloatTyID;
  const_float->pdata = malloc(sizeof(float));
  *((float *)const_float->pdata) = num;
  return const_float;
}

void *get_pdata(Value *this) { return this->pdata; }

/// All values are typed, get the type of this value.
Type *getType(Value *this) { return this->VTy; }

// enum ReplaceMetadataUses { No, Yes };
// void doRAUW(Value* this, Value *New, enum ReplaceMetadataUses
// ReplaceMetaUses)
// {
//     assert(New && "Value::replaceAllUsesWith(<null>) is invalid!");
//     assert(!contains(New, this) &&
//            "this->replaceAllUsesWith(expr(this)) is NOT valid!");
//     assert(getType(New) == getType(this) &&
//            "replaceAllUses of value with new value of different type!");

//     // Notify all ValueHandles (if present) that this value is going away.
//     if (this->HasValueHandle)
//         ValueHandleBase::ValueIsRAUWd(this, New);
//     if (ReplaceMetaUses == ReplaceMetadataUses::Yes && isUsedByMetadata())
//         ValueAsMetadata::handleRAUW(this, New);

//     while (!materialized_use_empty())
//     {
//         Use &U = *UseList;
//         // Must handle Constants specially, we cannot call replaceUsesOfWith
//         on a
//         // constant because they are uniqued.
//         if (auto *C = dyn_cast<Constant>(U.getUser()))
//         {
//             if (!isa<GlobalValue>(C))
//             {
//                 C->handleOperandChange(this, New);
//                 continue;
//             }
//         }

//         U.set(New);
//     }

//     if (BasicBlock *BB = dyn_cast<BasicBlock>(this))
//         BB->replaceSuccessorsPhiUsesWith(cast<BasicBlock>(New));
// }