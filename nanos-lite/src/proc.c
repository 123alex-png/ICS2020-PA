#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
Context *context_kload(PCB *pcb, void *entry, void *arg);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  // assert(arg);
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  context_kload(&pcb[0], (void *)hello_fun, (void *)12345);
  context_kload(&pcb[1], (void *)hello_fun, (void *)54321);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  naive_uload(NULL,"/bin/dummy");
}

Context* schedule(Context *prev) {

  current->cp = prev;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
