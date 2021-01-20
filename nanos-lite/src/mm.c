#include <memory.h>
#include <proc.h>
// #define PGSIZE 4096

static void *pf = NULL;
#define stdprot 0xffffffff

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  printf("pf = %p\n", pf);
  assert(pf < heap.end);
  return p;
}

static void* pg_alloc(int n) {
  size_t nr_page = n / PGSIZE;
  void *ret = new_page(nr_page);
  memset(ret, 0, (size_t)n);
  return ret;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  // assert(0);
  printf("brk: %p\n", (void *)brk);
  if(brk > current->max_brk){
    uintptr_t va = current->max_brk;
    if(va % PGSIZE != 0){
      void *pa = new_page(1);
      map(&(current->as), (void *)(ROUNDDOWN(va, PGSIZE)), pa, stdprot);
    }
    va = ROUNDUP(va, PGSIZE);
    while(va < brk){
      void *pa = new_page(1);
      map(&(current->as), (void *)va, pa, stdprot);
      va += PGSIZE;
    }
    current->max_brk = brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);
  vme_init(pg_alloc, free_page);
}
