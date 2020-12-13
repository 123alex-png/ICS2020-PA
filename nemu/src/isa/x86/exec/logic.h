#include "cc.h"
#define EFLAGS eflags

static inline def_EHelper(test) {
  rtl_and(s,s0,ddest,dsrc1);
  rtl_update_ZFSF(s,s0,id_dest->width);
  cpu.EFLAGS.CF=0;
  cpu.EFLAGS.OF=0;
  print_asm_template2(test);
}

static inline def_EHelper(and) {
  rtl_and(s,s0,ddest,dsrc1);
  rtl_update_ZFSF(s, s0, id_dest->width);
  cpu.EFLAGS.CF=0;
  cpu.EFLAGS.OF=0;
   //if(*s0==0x7961)printf("and\n");
  operand_write(s,id_dest,s0);
  print_asm_template2(and);
}

static inline def_EHelper(xor) {
 
 rtl_xor(s,s0,ddest,dsrc1);
 rtl_update_ZFSF(s, s0, id_dest->width);
  cpu.EFLAGS.CF=0;
  cpu.EFLAGS.OF=0;
   //if(*s0==0x7961)printf("xor\n");
  operand_write(s,id_dest,s0);
  print_asm_template2(xor);
}

static inline def_EHelper(or) {
  rtl_or(s,s0,ddest,dsrc1);
  rtl_update_ZFSF(s, s0, id_dest->width);
  cpu.EFLAGS.CF=0;
  cpu.EFLAGS.OF=0;
   //if(*s0==0x7961)printf("or\n%x\n",cpu.pc);
  operand_write(s,id_dest,s0);
 /* printf("%x\n",*ddest);*/
  print_asm_template2(or);
}

static inline def_EHelper(sar) {
  
  // unnecessary to update CF and OF in NEMU
  /*printf("%x\n",*ddest);*/

 
  rtl_sext(s,s0,ddest,id_dest->width);
  /* printf("%x\n",*s0);*/
  rtl_sar(s,ddest,s0,dsrc1);
  // if(*ddest==0x7961)printf("sar\n");
  operand_write(s,id_dest,ddest);
rtl_update_ZFSF(s,ddest,id_dest->width);
  print_asm_template2(sar);
}

static inline def_EHelper(shl) {
  rtl_shl(s,s0,ddest,dsrc1);
   //if(*s0==0x7961)printf("shl\n");
   operand_write(s,id_dest,s0);
  rtl_update_ZFSF(s,s0,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

static inline def_EHelper(shr) {
  /*printf("%x\n",*ddest);*/
  rtl_shr(s,s0,ddest,dsrc1);
 /* printf("%x\n%x\n%x\n",*ddest,*dsrc1,cpu.gpr[1]._16);*/
  //if(*s0==0x7961)printf("shr\n");
   operand_write(s,id_dest,s0);
  rtl_update_ZFSF(s,s0,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}


static inline def_EHelper(setcc) {
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, ddest, cc);
  operand_write(s, id_dest, ddest);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

static inline def_EHelper(not) {
  rtl_not(s,ddest,ddest);
  // if(*s0==0x7961)printf("not\n");
   operand_write(s, id_dest, ddest);
  print_asm_template1(not);
}

static inline def_EHelper(rol){
  *s0=*dsrc1 % (id_dest->width*8);
  *ddest=((*ddest)<<(*s0))|((*ddest)>>(id_dest->width*8-*(s0)));
   if(cpu.pc==0x3032366)printf("%x\n",cpu.pc);
  operand_write(s,id_dest,ddest);
}
static inline def_EHelper(ror){
  *s0=*dsrc1 % (id_dest->width*8);
  *ddest=((*ddest)>>(*s0))|((*ddest)<<(id_dest->width*8-*(s0)));
   if(cpu.pc==0x3032366)printf("%x\n",cpu.pc);
  operand_write(s,id_dest,ddest);
}
static inline def_EHelper(movsb){
  *s1=cpu.esi;
  *s2=cpu.edi;
 rtl_lm(s,s0,s1,0,1);
 rtl_sm(s,s2,0,s0,1);
 cpu.esi++;
 cpu.edi++;
  print_asm_template1(movsb);
}

static inline def_EHelper(movsw){
  *s1=cpu.esi;
  *s2=cpu.edi;
  if(s->isa.is_operand_size_16){
    rtl_lm(s,s0,s1,0,2);
    rtl_sm(s,s2,0,s0,2);
    cpu.esi+=2;
    cpu.edi+=2;
  }else{
    rtl_lm(s,s0,s1,0,4);
    rtl_sm(s,s2,0,s0,4);
    cpu.esi+=4;
    cpu.edi+=4;
  }
  print_asm_template1(movsw);
}