#include <stdio.h>
#include <NDL.h>
#include "assert.h"
int main() {
  NDL_Init(0);
  while (1) {
    char buf[64];
    printf("%s", buf);
    assert(0);
    if (NDL_PollEvent(buf, sizeof(buf))) {
      printf("receive event: %s\n", buf);
    }
  }
  return 0;
}
