int main() {
  int a = 111;
  int b = 222;
  int c = 333;
  int d = 444;
  int res = 0;
  while (a) {
    res = 888;
  }

  while (a && b || c && d) {
    res = a;
    if (b || c || a && d) {
      res = b;
    }
  }
  return res;
}