#ifndef USER_DEF
#define USER_DEF

#include "use.h"
#include "value.h"

// Use是依赖于User存在的(只有User知道它需要多少依赖以及依赖指向哪个Value),
// 因此User负责对Use的管理(申请与释放). User是一个通用的基础类,
// 而不同指令的操作数个数又不同, 导致Use不能作为User的成员存在(容器方式存储可以,
// 但是效率降低). 可以看到User并没有扩展额外的成员, 那么Use存储在哪里呢?
// 答案在新建User对象的时候.
typedef struct _User User;

struct _User {
  // user 继承 value
  Value value;

  // user 使用多个 value，在LLVM中， 通过函数 allocate_fixed_operand_user 及
  // user 的构造函数 （重载 new），在创建 user 的同时，创建了相应的
  // use，相关于在 user 之前有1个 use 或 多个 use llvm
  // 是隐式的创建，这里是显式的创建
  struct _Use *use_list;

  int num_oprands;
};

User *user_new();

User *user_new1(unsigned use_num);

int user_get_size(int use_num);

User *user_construct(void *place, int use_num);

Use *user_get_operand_use(User *this, unsigned i);

Value *user_get_value(User *this);

User *use_get_parent(Use *this);

#endif