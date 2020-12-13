#include "cc.h"


static inline def_EHelper(test) {
  rtl_and(s,s0,ddest,dsrc1);
  rtl_set_CF(s,rz);
  rtl_set_OF(s,rz);
  rtl_update_ZFSF(s, s0, id_dest->width);
  print_asm_template2(test);
}

static inline def_EHelper(and) {
  rtl_and(s,ddest,ddest,dsrc1);
  rtl_set_CF(s,rz);
  rtl_set_OF(s,rz);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s,id_dest,ddest);
  print_asm_template2(and);
}

static inline def_EHelper(xor) {
  rtl_xor(s,ddest,ddest,dsrc1);
  rtl_set_CF(s,rz);
  rtl_set_OF(s,rz);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s,id_dest,ddest);
  print_asm_template2(xor);
}

static inline def_EHelper(or) {
  rtl_or(s,ddest,ddest,dsrc1);
  rtl_set_CF(s,rz);
  rtl_set_OF(s,rz);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s,id_dest,ddest);
  print_asm_template2(or);
}

static inline def_EHelper(sar) {
  rtl_sext(s,ddest,ddest,id_dest->width);
  rtl_sar(s,ddest,ddest,dsrc1);
  rtl_update_ZFSF(s,ddest,id_dest->width);
  operand_write(s,id_dest,ddest);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

static inline def_EHelper(shl) {
  rtl_shl(s,ddest,ddest,dsrc1);
  rtl_update_ZFSF(s,ddest,id_dest->width);
  operand_write(s,id_dest,ddest);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

static inline def_EHelper(shr) {
  rtl_shr(s,ddest,ddest,dsrc1);
  rtl_update_ZFSF(s,ddest,id_dest->width);
  operand_write(s,id_dest,ddest);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

static inline def_EHelper(shld) {
  rtl_li(s,s1,*dsrc1&0x1f);
  if(*s1!=0){
    rtl_li(s,ddest,*ddest<<*s1);
    rtl_li(s,s0,*dsrc2>>(id_src2->width*8-*s1));
    rtl_add(s,ddest,ddest,s0);
    operand_write(s,id_dest,ddest);
  }
  rtl_update_ZFSF(s,ddest,id_dest->width);
}

static inline def_EHelper(shrd) {
  rtl_li(s,s1,*dsrc1&0x1f);
  if(*s1!=0){
    rtl_li(s,ddest,*ddest>>*s1);
    rtl_li(s,s0,*dsrc2 <<(id_src2->width*8-*s1));
    rtl_add(s,ddest,ddest,s0);
    operand_write(s,id_dest,ddest);
  }
  rtl_update_ZFSF(s,ddest,id_dest->width);
}


static inline def_EHelper(rol) {
  rtl_li(s,s1,*dsrc1%(id_dest->width*8));
  rtl_li(s,ddest,(*ddest<<*s1)|((*ddest)>>(id_dest->width*8-*s1)));
  // while(*s1!=0){
  //   rtl_msb(s,s0,ddest,id_dest->width);
  //   rtl_li(s,ddest,*ddest*2+*s0);
  //   rtl_li(s,s1,*s1-1);
  // }

  rtl_update_ZFSF(s,ddest,id_dest->width);
  operand_write(s,id_dest,ddest);
}

static inline def_EHelper(ror) {
  rtl_li(s,s1,*dsrc1%(id_dest->width*8));
  rtl_li(s,ddest,(*ddest>>*s1)|((*ddest)<<(id_dest->width*8-*s1)));
  // while(*s1!=0){
  //   //rtl_msb(s,s0,ddest,id_dest->width);
  //   rtl_li(s,s0,*ddest&0x1);
  //   rtl_li(s,ddest,(*ddest/2)+(*s0<<(id_dest->width*8-1)));
  //   rtl_li(s,s1,*s1-1);
  // }
  rtl_update_ZFSF(s,ddest,id_dest->width);
  operand_write(s,id_dest,ddest);
}

static inline def_EHelper(setcc) {
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, ddest, cc);
  operand_write(s, id_dest, ddest);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

static inline def_EHelper(not) {
  rtl_not(s,ddest,ddest);
  operand_write(s,id_dest,ddest);

  print_asm_template1(not);
}

