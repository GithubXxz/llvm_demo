#include <stdio.h>
int main() {
  int arr[10][20];
  int d[4][2] = {1, 2, {3}, {5}, 7, 8};
  for (int i = 0; i < 4; i++) {
    for (int m = 0; m < 2; m++)
      printf("\t%d", d[i][m]);
    printf("\n");
  }
  return 0;
}
