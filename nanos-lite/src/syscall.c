#include <common.h>
#include "syscall.h"
#include <sys/time.h>
#include <proc.h>


int fs_open(const char *pathname, int flags, int mode);
int fs_close(int fd);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
void naive_uload(PCB *pcb, const char *filename);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);
void switch_boot_pcb();

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
  fs_write(fd,buf,count);
  return count;
}

int sys_read(int fd, void *buf, size_t count){
  return fs_read(fd, buf, count);
}

size_t sys_lseek(int fd, size_t offset, int whence){
  return fs_lseek(fd,offset,whence);
}

intptr_t sys_brk(void * addr){
  return 0;
}

int sys_gettimeofday(struct timeval *tv, struct timezone *tz){
  uint64_t us;
  ioe_read(AM_TIMER_UPTIME,&us);
  tv->tv_usec=us%1000000;
  tv->tv_sec=us/1000000;
  return 0;
}

int sys_execve(const char *fname, char *argv[], char *envp[]){
  // naive_uload(NULL, fname);
  // printf("execve: %s\n", argv[0]);
  if(fs_open(fname,0,0)==-1){
    return -2;
  }
  context_uload(current, fname, argv, envp);//while(1);
  switch_boot_pcb();
  yield();
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
    case SYS_open:c->GPRx=fs_open((char *)a[1],0,0);break;
    case SYS_read:c->GPRx=fs_read((int)a[1],(void *)a[2],(size_t)a[3]);break;
    case SYS_write:c->GPRx=fs_write((int)a[1],(void *)a[2],(size_t)a[3]);break;
    case SYS_close:c->GPRx=fs_close(a[1]);break;
    case SYS_lseek:c->GPRx=fs_lseek((int)a[1],(size_t)a[2],(int)a[3]);break;
    case SYS_brk:c->GPRx=sys_brk((void *)a[1]);break;
    case SYS_execve:c->GPRx=sys_execve((char *)a[1], (char **)a[2], (char **)a[3]);break;
    case SYS_gettimeofday:c->GPRx=sys_gettimeofday((struct timeval *)a[1],(struct timezone *)a[2]);break;
    
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

