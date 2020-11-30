#include <common.h>
#include "syscall.h"
#include <sys/types.h>

extern char end;
void *prog_break=(void *)(&end);
int fs_open(const char *pathname, int flags, int mode);
int fs_close(int fd);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
off_t fs_lseek(int fd, off_t offset, int whence);

int sys_yield(){
  yield();
  return 0;
}

int sys_exit(){
  halt(0);
  return 0;
}

int sys_open(const char *pathname, int flags, int mode){
  int ret=fs_open(pathname,0,0);
  return ret;
}

int sys_close(int fd){
  return fs_close(fd);
}

int sys_write(int fd, void *buf, size_t count){
  switch(fd){
    case 0:{
      assert(0);
      break;
    }
    case 1:
    case 2:{
      for(size_t i=0;i<count;i++){
        putch(*(char *)(buf+i));
      }
      break;
    }
    default:{
      fs_write(fd,buf,count);
    }
  }
  return count;
}

int sys_read(int fd, void *buf, size_t count){
  switch(fd){
    case 0:
    case 1:
    case 2:{
      assert(0);
      break;
    }
    default:{
      return fs_read(fd,buf,count);
    }
  }
  return 0;
}

off_t sys_lseek(int fd,off_t offset, int whence){
  return fs_lseek(fd,offset,whence);
}

intptr_t sys_brk(void * addr){
  if(addr==0){
    return (intptr_t)prog_break;
  }
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
    case SYS_open:a[0]=sys_open((char *)a[1],0,0);break;
    case SYS_read:sys_read((int)a[1],(void *)a[2],(size_t)a[3]);break;
    case SYS_write:sys_write((int)a[1],(void *)a[2],(size_t)a[3]);break;
    case SYS_close:sys_close(a[1]);break;
    case SYS_lseek:sys_lseek((int)a[1],(off_t)a[2],(int)a[3]);
    case SYS_brk:sys_brk((void *)a[1]);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

