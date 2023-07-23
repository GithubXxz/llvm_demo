#include <stdio.h>
int main() {
  char buffer[30];
  float const_float_value = 3.3 > 3.3;
  float zero_float = 0;
  sprintf(buffer, "%f", zero_float);
  printf("%s", buffer);
  return 0;
}
