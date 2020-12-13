#include <monitor/difftest.h>
extern void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr);
static inline def_EHelper(lidt) {
  //TODO();
  rtl_li(s,s0,*dsrc1);
 cpu.idtr.limit=vaddr_read(*s0,2);
  if(s->isa.is_operand_size_16)cpu.idtr.base=vaddr_read(*s0+2,4)&0x00ffffff;
  else cpu.idtr.base=vaddr_read(*s0+2,4);
  print_asm_template1(lidt);
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
  //TODO();
 // printf("%x\n",*ddest);
  //raise_intr(s,*ddest,cpu.pc+2);
  raise_intr(s,*ddest,s->seq_pc);
  print_asm("int %s", id_dest->str);

#ifndef __DIFF_REF_NEMU__
  difftest_skip_dut(1, 2);
#endif
}

static inline def_EHelper(iret) {
 // TODO();


rtl_pop(s,s0);
//printf("%x\n",*s0);
rtl_pop(s,s1);
rtl_pop(s,s2);
cpu.eflag_val=*s2;
cpu.cs=*s1;
rtl_j(s,*s0);
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
  switch(id_dest->width){
    case 1:
     *s0=pio_read_b(*dsrc1);
     break;
    case 2:
     *s0=pio_read_w(*dsrc1);
     break;
     case 4:
     *s0=pio_read_l(*dsrc1);
     break;
  }
  operand_write(s,id_dest,s0);
  print_asm_template2(in);
  
}

static inline def_EHelper(out) {
 switch(id_src1->width){
    case 1:
     pio_write_b(*ddest,*dsrc1);
     break;
    case 2:
     pio_write_w(*ddest,*dsrc1);
     break;
     case 4:
     pio_write_l(*ddest,*dsrc1);
     break;
  }

  print_asm_template2(out);
}
