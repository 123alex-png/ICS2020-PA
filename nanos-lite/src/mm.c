#include <memory.h>
#include <proc.h>
// #define PGSIZE 4096

static void *pf = NULL;
#define stdprot 0xffffffff

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  // printf("pf = %p\n", pf);
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

// extern void *map_addr[4][0x4ffff];
/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  if(current->max_brk == 0){
    current->max_brk = brk;
    return -1;
  }
  else if(brk > current->max_brk){
    uintptr_t va = current->max_brk;
    // printf("va: %p\n", va);
    if(va % PGSIZE != 0){
      void *pa = 0;
      if(!pa){
        pa = new_page(1);
        map(&(current->as), (void *)ROUNDDOWN(va, PGSIZE), pa, stdprot);    
      }
    }
    else{
      void *pa = 0;
      pa = new_page(1);
      map(&(current->as), (void *)va, pa, stdprot);    
    }
    va = ROUNDDOWN(va, PGSIZE) + PGSIZE;
    while(va < brk){
      void *pa = 0;
      pa = new_page(1);
      map(&(current->as), (void *)va, pa, stdprot);    
      va += PGSIZE;
    }
    current->max_brk = brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  // printf("%p, %p\n", heap.start, heap.end);while(1);
  Log("free physical pages starting from %p", pf);
  vme_init(pg_alloc, free_page);
}
