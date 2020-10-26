#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  if (dest != src1) rtl_add(s, dest, src1, rz);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  rtl_li(s,dest,~(*src1));
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
  rtl_li(s,dest,-(*src1));
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  switch(width){
    case 4:rtl_li(s,dest,(int32_t)(*src1));break;
    case 2:rtl_li(s,dest,(int16_t)(*src1));break;
    case 1:rtl_li(s,dest,(int8_t)(*src1));break;
    default:assert(0);
  }
}


static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  switch(width){
    case 4:rtl_li(s,dest,(uint32_t)(*src1));break;
    case 2:rtl_li(s,dest,(uint16_t)(*src1));break;
    case 1:rtl_li(s,dest,(uint8_t)(*src1));break;
    default:assert(0);
  }
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  switch(width){
    case 4:rtl_li(s,dest,c_shr(*src1,31));break;
    case 2:rtl_li(s,dest,c_shr(*src1,15));break;
    case 1:rtl_li(s,dest,c_shr(*src1,7));break;
    default:assert(0);
  }
}

#endif
