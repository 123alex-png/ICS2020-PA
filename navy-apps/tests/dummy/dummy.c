#include <stdint.h>
#include <stdio.h>
#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main(int      argc, char    *argv[]) {
  // volatile char tmp = argv[0][0];
  // printf("&argv[0][4]=%p,argv[0][4] = %p\n",&argv[0][4], argv[0][4]&0xff);
  // printf("%s\n",argv[0]);
  // puts(argv[0]);
  return _syscall_(SYS_yield, 0, 0, 0);
}
