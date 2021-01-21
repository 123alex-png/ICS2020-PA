#include <cpu/exec.h>
#include "local-include/rtl.h"

#define IRQ_TIMER 32

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtlreg_t addr=cpu.idtr.base+8*NO;
  rtlreg_t low=vaddr_read(addr,4);
  rtlreg_t high=vaddr_read(addr+4,4);
  rtlreg_t entry=(high&0xffff0000)+(low&0xffff);
  rtl_push(s,(rtlreg_t *)&(cpu.eflags));
  cpu.eflags.IF = 0;
  rtl_push(s,(rtlreg_t *)&(cpu.cs));
  rtl_push(s,&(ret_addr));
  rtl_j(s,entry);
}

void query_intr(DecodeExecState *s) {
  if (cpu.eflags.IF && cpu.intr) {
    cpu.intr = false;
    raise_intr(s, IRQ_TIMER, cpu.pc);
    update_pc(s);
  }
}
