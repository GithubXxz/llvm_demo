// int array[110];
// int n;
// void init(int n) {
//   int i = 1;
//   while (i <= n * n + 1) {
//     array[i] = -1;
//     i = i + 1;
//   }
// }

// int findfa(int a) {
//   if (array[a] == a)
//     return a;
//   else {
//     array[a] = findfa(array[a]);
//     return array[a];
//   }
// }
// void mmerge(int a, int b) {
//   int m = findfa(a);
//   int n = findfa(b);
//   if (m != n)
//     array[m] = n;
// }
int main() {
  int t = 1;
  while (t) {
    t = t - 1;
    int i = 0;
    int fuck;
    while (i < fuck) {
      if (!t) {

        fuck = 1;
        if (i == 1) {
          fuck = 1;
        }
        if (i == fuck) {
          fuck = 1;
        }
        if (i < fuck && fuck != -1) {
          fuck = 1;
        }
        if (i > 1 && fuck != -1) {
          fuck = 1;
        }
        if (i < fuck && fuck - 1) {
          fuck = 1;
        }
        if (i > 1 && fuck != -1) {
          fuck = 1;
        }

        if (fuck != -1 && fuck != -1 && fuck == fuck) {
          fuck = 1;
        }
      }

      i = i + 1;
    }
    if (!i) {
      fuck = 1;
    }
  }
  return 0;
}
