#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

/* RTL pseudo instructions */

static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  if(src1==&(cpu.esp)){
    *s0=cpu.esp;
    cpu.esp-=4;
    rtl_sm(s,&cpu.esp,0,s0,4);
  }
  else{
    cpu.esp-=4;
    rtl_sm(s,&cpu.esp,0,src1,4);
  }
}

static inline def_rtl(pop, rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  rtl_lm(s,dest,&cpu.esp,0,4);
  cpu.esp+=4;
}

static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  rtl_msb(s,t0,src1,width);//src1的符号
  rtl_not(s,t1,src2);
  rtl_msb(s,t1,t1,width);//src2的符号
  rtl_msb(s,t2,res,width);//res的符号
  rtl_li(s,t0,interpret_relop(RELOP_EQ,*t0,*t1));//src1,src2是否符号相同
  rtl_li(s,t1,interpret_relop(RELOP_EQ,*t1,*t2));//src2,res是否符号相同
  rtl_not(s,t1,t1);
  rtl_li(s,t0,c_and(*t0,*t1));//OF
  rtl_mv(s,dest,t0);
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2)
  rtl_li(s,dest,interpret_relop(RELOP_LTU,*src1,*src2));
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  rtl_msb(s,t0,src1,width);//src1的符号
  rtl_msb(s,t1,src2,width);//src2的符号
  rtl_msb(s,t2,res,width);//res的符号
  rtl_li(s,t0,interpret_relop(RELOP_EQ,*t0,*t1));//src1,src2是否符号相同
  rtl_li(s,t1,interpret_relop(RELOP_EQ,*t1,*t2));//src2,res是否符号相同
  rtl_not(s,t1,t1);
  rtl_li(s,t0,c_and(*t0,*t1));//OF
  rtl_mv(s,dest,t0);
}

static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  rtl_li(s,t0,interpret_relop(RELOP_LTU,*res,*src1));
  rtl_mv(s,dest,t0);
}

#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
    cpu.eflags.f=*src;\
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
    *dest=cpu.eflags.f;\
  }

def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)

static inline def_rtl(update_ZF, const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  rtl_li(s,t0,c_sub(4,width));
  rtl_li(s,t0,c_mul_lo(*t0,4));
  rtl_li(s,t0,c_shr(0xffffffff,*t0));
  rtl_li(s,t0,!c_and(*result,*t0));
  rtl_set_ZF(s,t0);
}

static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  rtl_msb(s,t0,result,width);
  rtl_set_SF(s,t0);
}

static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}

#endif
