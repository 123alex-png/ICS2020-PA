#include <monitor/difftest.h>

void raise_intr(DecodeExecState *, uint32_t, vaddr_t);
extern rtlreg_t tss_addr;
static inline def_EHelper(lidt) {
  // if(s->isa.is_operand_size_16){
  //   cpu.idtr;
  // }
  rtl_lm(s,&(cpu.idtr.limit),&(cpu.eax),0,2);
  rtl_lm(s,&(cpu.idtr.base),&(cpu.eax),2,4);
  print_asm_template1(lidt);
}

static inline def_EHelper(lgdt) {
  rtl_lm(s,&(cpu.gdtr.limit),&(cpu.eax),0,2);
  rtl_lm(s,&(cpu.gdtr.base),&(cpu.eax),2,4);
  print_asm_template1(lgdt);
}

static inline def_EHelper(ltr) {
  rtl_lm(s,&(cpu.tr),&(cpu.eax),0,2);
  print_asm_template1(ltr);
}

static inline def_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src1->reg, 4), id_dest->reg);
}

static inline def_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src1->reg, reg_name(id_dest->reg, 4));

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(int) {
  raise_intr(s,*ddest,s->seq_pc);
  print_asm("int %s", id_dest->str);

#ifndef __DIFF_REF_NEMU__
  difftest_skip_dut(1, 2);
#endif
}

static inline def_EHelper(iret) {
  //TODO();
  rtl_pop(s,s0);
  rtl_j(s,*s0);
  rtl_pop(s,(rtlreg_t *)&(cpu.cs));
  rtl_pop(s,&(cpu.eflag_val));
  // printf("cs: %x\n", cpu.cs);
  if((cpu.cs & 0x3) == 0x3){//用户态
    // rtl_pop(s,s0);    
    // rtl_pop(s,(rtlreg_t *)&(cpu.ss));
    // rtl_mv(s, s0, &(cpu.esp));
    // vaddr_write(tss_addr+4, *s0, 4);
    printf("pop: esp = %x, ss = %x\n", cpu.esp, cpu.ss);
  }
  printf("iret: eflags: %x, eip: %x\n", cpu.eflag_val, cpu.pc);
  print_asm("iret");

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

static inline def_EHelper(in) {
  switch (id_dest->width)
  {
  case 1:
    *ddest=pio_read_b(*dsrc1);
    break;
  case 2:
    *ddest=pio_read_w(*dsrc1);
    break;
  case 4:
    *ddest=pio_read_l(*dsrc1);
    break;
  default:
    break;
  }
  

  print_asm_template2(in);
}

static inline def_EHelper(out) {
  switch (id_src1->width)
  {
  case 1:
    pio_write_b(*ddest,*dsrc1);
    break;
  case 2:
    pio_write_w(*ddest,*dsrc1);
    break;
  case 4:
    pio_write_l(*ddest,*dsrc1);
    break;
  default:
    break;
  }
  print_asm_template2(out);
}
