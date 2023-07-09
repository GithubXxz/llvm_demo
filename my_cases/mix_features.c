int global_a = 10;
int global_b = 20;
int use_global(int a, int b, int arr[]) {
  return 2 * global_a + a * global_b + b * arr[3];
}
int main() {
  int a = 111;
  int b = 222;
  int arr[10][20];
  if (a == 111) {
    arr[3][3] = 333;
    b = 444;
    global_a = 555;
  } else {
    arr[3][3] = 666;
    global_a = 777;
    a = 999;
  }
  return a + global_a + 2 * use_global(b, global_b, arr[3]);
}
