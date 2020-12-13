#define EFLAGS eflags
static inline def_EHelper(mov) {
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(mov);
}

static inline def_EHelper(push) {
  *s0=*ddest;
  rtl_push(s,s0);
 // printf("%x\n",*s0);
  print_asm_template1(push);
}

static inline def_EHelper(pop) {
 // printf("%x\n",cpu.esp);
  rtl_pop(s,s0);
  *ddest=*s0;
  print_asm_template1(pop);
}

static inline def_EHelper(pusha) {
  //TODO();
if(s->isa.is_operand_size_16){
  rtl_lr(s,s0,4,2);
  rtl_lr(s,s1,0,2);
  rtl_push(s,s1);
  rtl_lr(s,s1,1,2);
  rtl_push(s,s1);
  rtl_lr(s,s1,2,2);
  rtl_push(s,s1);
  rtl_lr(s,s1,3,2);
  rtl_push(s,s1);
  rtl_push(s,s0);
  rtl_lr(s,s1,5,2);
  rtl_push(s,s1);
  rtl_lr(s,s1,6,2);
  rtl_push(s,s1);
  rtl_lr(s,s1,7,2);
  rtl_push(s,s1);
}else{
  rtl_lr(s,s0,4,4);
  rtl_lr(s,s1,0,4);
  rtl_push(s,s1);
  rtl_lr(s,s1,1,4);
  rtl_push(s,s1);
  rtl_lr(s,s1,2,4);
  rtl_push(s,s1);
  rtl_lr(s,s1,3,4);
  rtl_push(s,s1);
  rtl_push(s,s0);
  rtl_lr(s,s1,5,4);
  rtl_push(s,s1);
  rtl_lr(s,s1,6,4);
  rtl_push(s,s1);
  rtl_lr(s,s1,7,4);
  rtl_push(s,s1);
}
  print_asm("pusha");
}

static inline def_EHelper(popa) {
  //TODO();
if(s->isa.is_operand_size_16){
  rtl_pop(s,s0);
  rtl_sr(s,7,s0,2);
  rtl_pop(s,s0);
  rtl_sr(s,6,s0,2);
  rtl_pop(s,s0);
  rtl_sr(s,5,s0,2);
  rtl_pop(s,s0);
  rtl_pop(s,s0);
  rtl_sr(s,3,s0,2);
  rtl_pop(s,s0);
  rtl_sr(s,2,s0,2);
  rtl_pop(s,s0);
  rtl_sr(s,1,s0,2);
  rtl_pop(s,s0);
  rtl_sr(s,0,s0,2);
}else{
  rtl_pop(s,s0);
  rtl_sr(s,7,s0,4);
  rtl_pop(s,s0);
  rtl_sr(s,6,s0,4);
  rtl_pop(s,s0);
  rtl_sr(s,5,s0,4);
  rtl_pop(s,s0);
  rtl_pop(s,s0);
  rtl_sr(s,3,s0,4);
  rtl_pop(s,s0);
  rtl_sr(s,2,s0,4);
  rtl_pop(s,s0);
  rtl_sr(s,1,s0,4);
  rtl_pop(s,s0);
  rtl_sr(s,0,s0,4);
}
  print_asm("popa");
}

static inline def_EHelper(leave) {

    cpu.esp=cpu.ebp;
    rtl_pop(s,&cpu.ebp);
  
  print_asm("leave");
}

static inline def_EHelper(cltd) {
  if (s->isa.is_operand_size_16) {
    rtl_lr(s,s0,0,2);
    rtl_sext(s,s0,s0,2);
    rtl_sari(s,s0,s0,16);
    rtl_sr(s,2,s0,2);
  }
  else {
    rtl_lr(s,s0,0,4);
    rtl_sext(s,s0,s0,4);
    rtl_sari(s,s0,s0,31);
    rtl_sr(s,2,s0,4);
  }

  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

static inline def_EHelper(cwtl) {
  if (s->isa.is_operand_size_16) {
   rtl_lr(s,s0,0,1);
    rtl_sext(s,s0,s0,1);
    rtl_sr(s,0,s0,2);
  }
  else {
    rtl_lr(s,s0,0,2);
    rtl_sext(s,s0,s0,2);
    rtl_sr(s,0,s0,4);
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
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(movzx);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  operand_write(s, id_dest, ddest);
  print_asm_template2(lea);
}
static inline def_EHelper(xchg){//trouble
  if(id_dest->width==4){
    *s0=cpu.eax;
  cpu.eax=*ddest;
  *ddest=*s0;}else
  {*s0=cpu.gpr[0]._16;
  cpu.gpr[0]._16=*ddest;
  *ddest=*s0;
    }
  
  print_asm_template1(xchg)
}
static inline def_EHelper(bsr){
  *s0=*dsrc1;
  *s1=0;
  if(*s0==0)cpu.EFLAGS.ZF=1;
  else{cpu.EFLAGS.ZF=0;}
  while(*s0){
     *s0=(*s0)>>1;
     (*s1)++;
  }
  (*s1)--;
  *ddest=*s1;
  print_asm_template2(bsr);
}
static inline def_EHelper(shld){
  //printf("%x\n",*ddest);
  *s2=*dsrc1&0x1f;
 if(*s2){
    cpu.EFLAGS.CF=(*ddest)&(1<<(id_dest->width-*s2));
   *s0=*ddest<<*s2;
   *s1=*dsrc2>>((id_src2->width)*8-*s2);
   *s0=*s0|*s1;
   //printf("%x\n",*s0);
   // if(cpu.pc==0x3032366)printf("%x\n",cpu.pc);
   rtl_update_ZFSF(s,s0,id_dest->width);
   operand_write(s,id_dest,s0);
 }

  print_asm_template3(shld);
}

static inline def_EHelper(shrd){
 // printf("%x\n",*ddest);
  *s2=*dsrc1&0x1f;
 if(*s2){
   cpu.EFLAGS.CF=(*ddest)&(1<<(*s2-1));
   *s0=*ddest>>*s2;
   *s1=*dsrc2<<((id_src2->width)*8-*s2);
   *s0=*s0|*s1;
   //printf("%x\n",*s0);
   // if(cpu.pc==0x3032366)printf("%x\n",cpu.pc);
    rtl_update_ZFSF(s,s0,id_dest->width);
   operand_write(s,id_dest,s0);
 }
  print_asm_template3(shrd);
}
