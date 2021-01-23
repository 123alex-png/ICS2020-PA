#include <cpu/exec.h>
#include "local-include/rtl.h"

#define IRQ_TIMER 32
rtlreg_t ksp = 1;
rtlreg_t tss_addr;
void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  printf("tr: %d\n", cpu.tr);
  
  if((cpu.cs & 0x3) == 0x3){
    // assert(0);
    rtlreg_t gdt_addr=cpu.gdtr.base+8*cpu.tr;
    rtlreg_t base_15_0 = vaddr_read(gdt_addr+2, 2) & 0xffff;
    rtlreg_t base_23_16 = vaddr_read(gdt_addr+4, 1) & 0xff;
    rtlreg_t base_31_24 = vaddr_read(gdt_addr+7, 1) & 0xff;
    // printf("gdtr: %x, %x\n", cpu.gdtr.base, vaddr_read(cpu.gdtr.base, 4));
    // printf("%x, %x, %x\n", base_15_0, base_23_16, base_31_24);
    tss_addr = (base_15_0) | (base_23_16 << 16) | (base_31_24 << 24);
    printf("tss_addr: %x\n,", tss_addr);
    ksp = vaddr_read(tss_addr+4, 4);//tss.esp0
    printf("ksp: %x, cs: %x\n", ksp, cpu.cs);

    rtl_mv(s, (rtlreg_t *)&(cpu.esp), s0);
    rtl_mv(s, (rtlreg_t *)&(cpu.ss), s1);
    
    rtl_mv(s, (rtlreg_t *)&ksp, &(cpu.esp));
    rtl_push(s, s1);
    rtl_push(s, s0);
    printf("push: esp = %x, ss = %x\n", *s0, *s1);
    // while(1);
  }
  vaddr_write(tss_addr+4, 0, 4);
  rtlreg_t addr=cpu.idtr.base+8*NO;
  rtlreg_t low=vaddr_read(addr,4);
  rtlreg_t high=vaddr_read(addr+4,4);
  rtlreg_t entry=(high&0xffff0000)+(low&0xffff);
  rtl_push(s,(rtlreg_t *)&(cpu.eflags));
  cpu.eflags.IF = 0;
  rtl_push(s,(rtlreg_t *)&(cpu.cs));
  rtl_push(s,&(ret_addr));
  // printf("yield: eflags: %x, eip: %x\n", cpu.eflag_val, cpu.pc);
  
  rtl_j(s,entry);
}



void query_intr(DecodeExecState *s) {
  if (cpu.eflags.IF && cpu.intr) {
    cpu.intr = false;
    raise_intr(s, IRQ_TIMER, cpu.pc);
    update_pc(s);
  }
}
