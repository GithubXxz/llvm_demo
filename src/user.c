#include "user.h"

#include <assert.h>
#include <stdlib.h>

#include "use.h"

User *user_new() {
  int size = sizeof(User);
  void *Storage = malloc(size);
  // User是如何管理Use对象的了
  // 第一种是独立分配, 在构造User对象时额外分配一个指针用来保存Use数组,
  // 这种情况下HasHungOffUses为true.
  //   然后将返回地址的起始作为Use指针并置空,
  //   之后的地址作为User对象的地址初始化并返回.
  //   即在每个User对象之前有一个Use指针大小的空间保存了一个默认为空的Use指针.
  // Use **HungOffOperandList = (Use **)(Storage);
  // User *Obj = (User *)(HungOffOperandList + 1);
  User *Obj = (User *)(Storage);
  (Obj->use_list) = Storage;
  (Obj->value).NumUserOperands = 0;
  (Obj->value).HasHungOffUses = true;
  return Obj;
}

User *user_new1(unsigned use_num) {
  // 另一种是 co-allocated,
  // 在构造User对象时传入边的数量并分配连续内存同时保存User与Use,
  assert(use_num < (1u << NumUserOperandsBits) && "Too many operands");

  uint8_t *Storage = (uint8_t *)malloc(sizeof(User) + sizeof(Use) * use_num);

  return user_construct(Storage, use_num);
}

// 将user内存分块 place是占用内存的首地址 use是需要分配的use的个数
User *user_construct(void *place, int use_num) {
  // 在place地方建立一个user，内存已分配好
  Use *Start = (Use *)(place);
  Use *End = Start + use_num;
  User *Obj = (User *)(End);
  // 将uselist执行第一条USE边的内存
  Obj->use_list = Start;
  Obj->num_oprands = use_num;
  ((Value *)Obj)->HasHungOffUses = false;

  for (; Start != End; Start++) {
    // 在 Start 地址上用构造 Use对象
    // 创建use对象让use对象的parent指向该user
    use_create(Start, Obj);
  }
  // 返回值是中间地址 低地址是use的地址 高地址是value的地址
  return Obj;
}

int user_get_size(int use_num) {
  if (use_num > 0)
    return sizeof(User) + sizeof(Use) * use_num;
  else
    return sizeof(User);
}

Use *user_get_operand_use(User *this, unsigned ind) {
  // 通过数组来表示地址偏移量
  return this->use_list + ind;
}

Use *get_operand(User *this, int ind) {
  if (this->value.HasHungOffUses) {
    return this->use_list;
  } else {
    assert(this->value.NumUserOperands > ind && "operand index overflow");
    return this->use_list + ind;
  }
}

Value *user_get_value(User *this) { return (Value *)this; }

/**
 * 再来看下User如何访问Use对象.
 */

// Use *getHungOffOperands(User* this) {
//     //return *(reinterpret_cast<const Use *const *>(this) - 1);
//     return *((const Use **)(this) - 1);
// }

Use *getHungOffOperands(User *this) {
  // return *(reinterpret_cast<Use **>(this) - 1);
  return *((Use **)(this) - 1);
}

User *use_get_parent(Use *this) { return this->Parent; };