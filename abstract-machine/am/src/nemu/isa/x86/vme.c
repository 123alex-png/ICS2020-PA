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
}


//envp未实现
Context* ucontext(AddrSpace *as, Area kstack, void *entry, char *const argv[], char *const envp[]) {
  Context *ret = (Context *)(kstack.end) - 1;
  ret -> eip = (uintptr_t)entry;
  ret -> cs = 0x8;
  ret -> esp = (uintptr_t)kstack.end;
  if(argv != NULL){
    intptr_t *argp = kstack.end - 0x80;
    printf("argp = %p\n", argp);
    int argc = 1;
    char *last= (char *)argp + 0x30;
    for(; /*argv[argc]!=NULL*/argc<=1; argc++){
      argp[argc] = (intptr_t)last;
      printf("argp[%d] = %p\n", argc, argp[argc]);
      last += strlen(argv[argc-1]);
    }
    argp[argc] = 0;
    --argc;
    argp[0] = argc;
    ret -> GPRx = (uintptr_t)argp;
    char *end = (char *)argp + 0x30;//至多可放12个参数，所有参数长度和至多80字节
    for(int i = 0; i < argc; i++){
      for(int j = 0; j < strlen(argv[i]); j++){
        *end++ = argv[i][j];
        printf("end = %p, *end = %x\n", end - 1, *(end-1));
      }
    }
  }
  else{
    ret -> GPRx = 0;
  }
  return ret;
}
