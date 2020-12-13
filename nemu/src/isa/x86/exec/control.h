// #include "cc.h"

// static inline def_EHelper(jmp) {
//   // the target address is calculated at the decode stage
//   rtl_j(s, s->jmp_pc);

//   print_asm("jmp %x", s->jmp_pc);
// }

// static inline def_EHelper(jcc) {
//   // the target address is calculated at the decode stage
//   uint32_t cc = s->opcode & 0xf;
//   rtl_setcc(s, s0, cc);
//   rtl_jrelop(s, RELOP_NE, s0, rz, s->jmp_pc);

//   print_asm("j%s %x", get_cc_name(cc), s->jmp_pc);
// }

// static inline def_EHelper(jmp_rm) {
//   rtl_jr(s, ddest);

//   print_asm("jmp *%s", id_dest->str);
// }

// static inline def_EHelper(call) {
//   // the target address is calculated at the decode stage
//   rtl_push(s,&(s->seq_pc));
//   rtl_j(s,s->jmp_pc);
//   print_asm("call %x", s->jmp_pc);
// }

// static inline def_EHelper(ret) {
//   rtl_pop(s,s0);
//   rtl_j(s,*s0);
//   print_asm("ret");
// }

// static inline def_EHelper(ret_imm) {
//   TODO();

//   print_asm("ret %s", id_dest->str);
// }

// static inline def_EHelper(call_rm) {
//   rtl_push(s,&(s->seq_pc));
//   rtl_li(s,s0,*ddest);
//   rtl_j(s,*s0);
//   print_asm("call *%s", id_dest->str);
// }
#include "cc.h"

static inline def_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(s, s->jmp_pc);

  print_asm("jmp %x", s->jmp_pc);
}

static inline def_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, s0, cc);
  rtl_jrelop(s, RELOP_NE, s0, rz, s->jmp_pc);
 /* printf("%x\n",s->jmp_pc);*/
  print_asm("j%s %x", get_cc_name(cc), s->jmp_pc);
}

static inline def_EHelper(jmp_rm) {
  rtl_jr(s, ddest);
  print_asm("jmp *%s", id_dest->str);
}

static inline def_EHelper(call) {
  // the target address is calculated at the decode stage
  rtl_push(s,&s->seq_pc);
  rtl_j(s,s->jmp_pc);
  print_asm("call %x", s->jmp_pc);
}

static inline def_EHelper(ret) {
 rtl_pop(s,s0);
 /*printf("%x\n",*s0);*/
 rtl_j(s,*s0);

  print_asm("ret");
}

static inline def_EHelper(ret_imm) {
  TODO();

  print_asm("ret %s", id_dest->str);
}

static inline def_EHelper(call_rm) {//incompleted
  
   if(*ddest==0){
     printf("%x\n",cpu.pc);
     printf("%x\n",cpu.edx);
   }
   /*if(cpu.pc==0x12cbd8){
     printf("%x\n",*ddest);
     printf("%x\n",cpu.edx);
   }*/
  rtl_push(s,&s->seq_pc);
   rtl_jr(s,ddest);
  print_asm("call *%s", id_dest->str);
}
