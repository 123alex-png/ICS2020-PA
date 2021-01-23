#include <am.h>
#include <x86.h>
#include <klib.h>

#define NR_IRQ         256     // IDT size
#define SEG_KCODE      1
#define SEG_KDATA      2
#define NR_SEG 6
static SegDesc gdt[NR_SEG] = {};
static TSS32 tss = {};

static Context* (*user_handler)(Event, Context*) = NULL;
uintptr_t get_ksp();
void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();

void __am_kcontext_start();

void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

Context* __am_irq_handle(Context *c) {
  // uint32_t *x = (uint32_t *)(c + 1);//ksp
  tss.esp0 = 0;
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->irq) {
      case 0x80:ev.event=EVENT_SYSCALL;break;
      case 0x81:ev.event=EVENT_YIELD;break;
      case 0x20:ev.event=EVENT_IRQ_TIMER;break;
      default: ev.event = EVENT_ERROR; break;
    }
    // printf("c->esp: %p, cs: %p\n", c->esp, c->cs);
    c = user_handler(ev, c);
    // printf("c->esp: %p, cs: %p\n", c->esp, c->cs);
    // static int cnt = 0;
    // assert(++cnt=10);
    assert(c != NULL);
  }
  __am_switch(c);
  if(((c->cs) & 0x3) == 0x3){
    
    uint32_t t;
    asm volatile("mov %%esp, %0":"=r"(t));//c->sp = $sp;
    tss.esp0 = t;
    // printf("t: %p, ksp: %p, &ksp: %p\n", t, tss.esp0, &(tss.esp0));
    // while(1);
  }
  return c;
}

bool cte_init(Context*(*handler)(Event, Context*)) {
  static GateDesc32 idt[NR_IRQ];

  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i]  = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
  }

  // ----------------------- interrupts ----------------------------
  idt[32]   = GATE32(STS_IG, KSEL(SEG_KCODE), __am_irq0,    DPL_KERN);
  // ---------------------- system call ----------------------------
  idt[0x80] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecsys,  DPL_USER);
  idt[0x81] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  // initialize GDT
  gdt[1] = SEG32(STA_X | STA_R,   0,      0xffffffff, DPL_KERN);
  gdt[2] = SEG32(STA_W,           0,      0xffffffff, DPL_KERN);
  gdt[3] = SEG32(STA_X | STA_R,   0,      0xffffffff, DPL_USER);
  gdt[4] = SEG32(STA_W,           0,      0xffffffff, DPL_USER);
  gdt[5] = SEG16(STS_T32A,     &tss, sizeof(tss) - 1, DPL_KERN);
  set_gdt(gdt, sizeof(gdt[0]) * NR_SEG);

  // initialize TSS
  tss.ss0 = KSEL(2);
  set_tr(KSEL(5));
  // printf("tss.esp0: %p\n", tss.esp0);
  // printf("KSL(5): %d\n", KSEL(5));
  return true;
}


Context* kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *ret = (Context *)(kstack.end) - 1;
  ret -> GPR1 = (uintptr_t)arg;
  ret -> GPR2 = (uintptr_t)entry;
  ret -> cs = KSEL(1);
  ret -> eflags |= 1<<9;
  ret -> eip = (uintptr_t)__am_kcontext_start;
  return ret;
}

void yield() {
  asm volatile("int $0x81");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}