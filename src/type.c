#include "type.h"

#include "stdio.h"

// 初始化type 修改指针里面的值而不是修改指针
void type_init(Type **this_addr) {
  printf("OX%p\n", this_addr);
  Type *this = *this_addr;
  this = (Type *)malloc(sizeof(Type));
  printf("OX%p\n", this);
  this->TID = VoidTyID;
  this->NumContainedTys = 1;
}

void new_type(Type *this, TypeID tid) {
  this->TID = tid;
  this->SubclassData = 0;
}

unsigned getSubclassData(Type *this) { return this->SubclassData; }

void setSubclassData(Type *this, unsigned val) {
  this->SubclassData = val;
  // Ensure we don't have any accidental truncation.
  // assert(getSubclassData() == val && "Subclass data too large for field");
}

/// Return true if this is one of the six floating-point types
// bool isFloatingPointTy(Type *this)
//{
//     return getTypeID(this) == HalfTyID || getTypeID(this) == BFloatTyID ||
//            getTypeID(this) == FloatTyID || getTypeID(this) == DoubleTyID ||
//            getTypeID(this) == X86_FP80TyID || getTypeID(this) == FP128TyID ||
//            getTypeID(this) == PPC_FP128TyID;
// }

////////////////////////////////////////////////////////////////////////////////////////////