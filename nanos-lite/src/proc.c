#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
PCB *fg_pcb = &pcb[1];
void context_kload(PCB *pcb, void *entry, void *arg);
void context_uload(PCB *pcb, char *filename, char *const argv[], char *const envp[]);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void switch_fg_pcb(int id){
  fg_pcb = &pcb[id];
}

void hello_fun(void *arg) {
  // assert(arg);
  // int j = 1;
  int cnt=0;
  while (1) {
    // if(j>=1){
      Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, ++cnt);
      // j=1;
    // }
    // j = 1;
    yield();
  }
}

void init_proc() {
  // context_kload(&pcb[0], (void *)hello_fun, (void *)12345);
  // context_kload(&pcb[1], (void *)hello_fun, (void *)52314);
  
  // char *arg[]={/*"/bin/exec-test", "12", "/bin/menu",*/NULL};
  char *arg[]={"/bin/exec-test", "12", "/bin/menu", NULL};
  context_uload(&pcb[0], "/bin/pal", arg, NULL);
  // context_uload(&pcb[0], "/bin/hello", arg, NULL);
  // context_uload(&pcb[1], "/bin/pal", arg, NULL);
  // context_uload(&pcb[2], "/bin/bird", arg, NULL);
  // context_uload(&pcb[3], "/bin/nslider", arg, NULL);

  switch_boot_pcb();
  Log("Initializing processes...");
}

Context* schedule(Context *prev) {

  current->cp = prev;
  // current = (current == &pcb[0] ? fg_pcb : &pcb[0]);
  current = &pcb[0];
  return current->cp;
}
