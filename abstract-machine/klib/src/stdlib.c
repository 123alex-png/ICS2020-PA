#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;
extern char _heap_start;
char *hbrk=&_heap_start;
int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

void *malloc(size_t size) {
  // address+=size;
  // return address;
  // size  = (size_t)ROUNDUP(size, 8);
  // printf("heap: %p\n", &_heap_start);
  char *old = hbrk+0x10000000;
  hbrk += size;
  //assert((uintptr_t)heap.start <= (uintptr_t)hbrk && (uintptr_t)hbrk < (uintptr_t)heap.end);
  for (char *p = old; p != hbrk; p ++) {
    *p = 0;
  }
  //assert((uintptr_t)hbrk - (uintptr_t)heap.start <= setting->mlim);
  return old;
}

void free(void *ptr) {
}

#endif
