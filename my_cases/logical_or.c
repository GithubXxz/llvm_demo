int logical_or() {
  const int a = 233;
  const int b = 323;
  int res = 0;
  if (a > 200 || b > 500) {
    res = 10086;
  } else {
    res = 2333333;
  }
  return res;
};
