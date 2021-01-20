#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;
extern char _stack_pointer;
static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0xc0000000)

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    
    for (; va < segments[i].end; va += PGSIZE) {
      // if((uint32_t)va<=0x2300000)
      // printf("va: %p\n",va);
      map(&kas, va, va, 0);
    }
  }

  set_cr3(kas.ptr);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->cr3 = (vme_enable ? (void *)get_cr3() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->cr3 != NULL) {
    set_cr3(c->cr3);
  }
}


void map(AddrSpace *as, void *va, void *pa, int prot) {
  // printf("va = %p\n", va);
  // printf("pa = %p\n", pa);
  // assert((uintptr_t)va % PGSIZE == 0);
  assert((uintptr_t)pa % PGSIZE == 0);
  assert(as != NULL);
  // assert(va == pa);//only for nanos
  uintptr_t *pgdir = (uintptr_t *)as->ptr;
  for(int i = 0; i <= 0x3ff; i++){
    // printf("pgdir[%d]: %p\n", i, pgdir[i]);
    assert(pgdir[i]==0);
  }
  assert(0);
  size_t pgdir_index = (((uint32_t)va)>>22)&0x3ff;//高10位
  if((pgdir[pgdir_index] & PTE_P)==0){//装入位是0才填入
    uintptr_t new_pgtab = (uintptr_t)pgalloc_usr(PGSIZE);
    printf("pgdir: %p, new_pagetab: %p, va: %p, pa: %p\n",(void *)pgdir, (void *)new_pgtab, va, pa);
    pgdir[pgdir_index] = (new_pgtab | PTE_P);//entry只使用new_pgtab的高20位，或起来不会产生影响
  }
  assert((pgdir[pgdir_index] & PTE_P)==1);
  //到此保证了pgdir[pgdir_index]装入位为1
  uintptr_t *pgtab = (uintptr_t *)(pgdir[pgdir_index] & ~0xfff);//高20位
  
  size_t pgtab_index = ((uint32_t)va >> 12) & 0x3ff;//中间10位
  // printf("pgdir_index = %d, pgtab_index = %d\n", pgdir_index, pgtab_index);
  // printf("pgdir = %p, pgtab = %p\n", pgdir, pgtab);
  // printf("pgtab[%d] = %p\n", pgtab_index, pgtab[pgtab_index]);
  if((pgtab[pgtab_index] & PTE_P) == 1){//是否存在这种情况？？？
    // panic("PTE_P of pgtab has been 1");
  }
  pgtab[pgtab_index] = ((uintptr_t)pa & ~0xfff) | PTE_P;
}


Context* ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *ret = (Context *)(kstack.end) - 1;
  ret -> cr3 = as->ptr;
  ret -> eip = (uintptr_t)entry;
  ret -> cs = 0x8;
  ret -> esp = (uintptr_t)heap.end;
  return ret;
}
