#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  for(int i=0;i<=129;i++){
  rtlreg_t addr=cpu.idtr.base+8*i;
  rtlreg_t low=vaddr_read(addr,4);
  rtlreg_t high=vaddr_read(addr+4,4);
  rtlreg_t entry=(high&0xffff0000)+(low&0xffff);
  printf("idt[%d]=0x%x\n",i, entry);
  }
  rtlreg_t addr=cpu.idtr.base+8*NO;
  rtlreg_t low=vaddr_read(addr,4);
  rtlreg_t high=vaddr_read(addr+4,4);
  rtlreg_t entry=(high&0xffff0000)+(low&0xffff);
  rtl_push(s,(rtlreg_t *)&(cpu.eflags));
  rtl_push(s,(rtlreg_t *)&(cpu.cs));
  rtl_push(s,&(ret_addr));
  rtl_j(s,entry);
}

void query_intr(DecodeExecState *s) {
  TODO();
}
