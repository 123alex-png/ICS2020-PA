#include <common.h>
#include "syscall.h"

extern char end;
void *prog_break=(void *)(&end);

int sys_yield(){
  yield();
  return 0;
}

int sys_exit(){
  halt(0);
  return 0;
}

int sys_write(int fd, void *buf, size_t count){
  switch(fd){
    case 1:
    case 2:{
      for(size_t i=0;i<count;i++){
        putch(*(char *)(buf+i));
      }
    }
  }
  return count;
}

int sys_brk(void * addr){
  prog_break=addr;
  return 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case SYS_exit:sys_exit();break;
    case SYS_yield:sys_yield();break;
    case SYS_write:sys_write((int)a[1],(void *)a[2],(size_t)a[3]);break;
    case SYS_brk:sys_brk((void *)a[1]);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

