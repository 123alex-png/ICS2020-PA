#include <common.h>
#include "syscall.h"

uintptr_t sys_yield(){
  yield();
  return 0;
}

uintptr_t sys_exit(){
  halt(0);
  return 0;
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit:sys_exit();break;
    case SYS_yield:sys_yield();break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

