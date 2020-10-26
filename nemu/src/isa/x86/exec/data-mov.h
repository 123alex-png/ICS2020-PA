static inline def_EHelper(mov) {
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(mov);
}

static inline def_EHelper(push) {
  rtl_push(s,ddest);
  print_asm_template1(push);
}

static inline def_EHelper(pop) {
  rtl_pop(s,ddest);

  print_asm_template1(pop);
}

static inline def_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

static inline def_EHelper(popa) {
  TODO();

  print_asm("popa");
}

static inline def_EHelper(leave) {
  rtl_mv(s,&cpu.esp,&cpu.ebp);
  rtl_pop(s,&cpu.ebp);
  print_asm("leave");
}

static inline def_EHelper(cltd) {
  if (s->isa.is_operand_size_16) {
    *s0=c_shr(cpu.ax,15);
    if(*s0==0){
      cpu.dx=0x0;
    }
    else{
      cpu.dx=0xffff;
    }
  }
  else {
    rtl_msb(s,s0,&cpu.eax,4);
    if(*s0==0){
      rtl_li(s,&cpu.edx,0x0);
    }
    else{
      rtl_li(s,&cpu.edx,0xffffffff);
    }
  }

  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

static inline def_EHelper(cwtl) {
  if (s->isa.is_operand_size_16) {
  TODO();
  }
  else {
  TODO();
  }

  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movsx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(movsx);
}

static inline def_EHelper(movzx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_zext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(movzx);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  operand_write(s, id_dest, ddest);
  print_asm_template2(lea);
}

// static inline def_EHelper(xchg){
//   if(s->isa.is_operand_size_16){
//     rtl_li(s,s0,cpu.ax);
//     rtl_li(s,&cpu.ax,*(uint16_t*)ddest);
//     rtl_li(s,ddest,s0);
//   }
//   else{
//     rtl_mv(s,s0,&(cpu.eax));
//     rtl_mv(s,&(cpu.eax),ddest);
//     rtl_mv(s,ddest,s0);
//   }
// }
