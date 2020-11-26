#include <common.h>
#include "syscall.h"

uintptr_t sys_yield(){
  yield();
  return 0;
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case 1:sys_yield();break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

