#include "../local-include/rtl.h"

#include "arith.h"
#include "control.h"
#include "data-mov.h"
#include "logic.h"
#include "string.h"
#include "system.h"

def_EHelper(nop);
def_EHelper(inv);
def_EHelper(nemu_trap);
def_EHelper(call);
def_EHelper(push);
def_EHelper(pop);
def_EHelper(sub);
def_EHelper(xor);
def_EHelper(ret);
def_EHelper(lea);
def_EHelper(and);
def_EHelper(add);
def_EHelper(cmp);
def_EHelper(setcc);
def_EHelper(movzx);
def_EHelper(jcc);
def_EHelper(leave);
def_EHelper(inc);
def_EHelper(adc);
def_EHelper(or);
def_EHelper(sar);
def_EHelper(shl);
def_EHelper(test);
def_EHelper(not);
def_EHelper(dec);
def_EHelper(imul1);
def_EHelper(imul2);
def_EHelper(imul3);
def_EHelper(cltd);
def_EHelper(idiv);
def_EHelper(jmp);
def_EHelper(movsx);
def_EHelper(call_rm);
def_EHelper(jmp_rm);
def_EHelper(shr);
def_EHelper(mul);
def_EHelper(sbb);
def_EHelper(in);
def_EHelper(out);
def_EHelper(neg);
def_EHelper(xchg);
def_EHelper(bsr);
def_EHelper(rol);
def_EHelper(movsb);
def_EHelper(movsw);
def_EHelper(ror);
def_EHelper(shld);
def_EHelper(lidt);
def_EHelper(int);
def_EHelper(pusha);
def_EHelper(popa);
def_EHelper(iret);
def_EHelper(shrd);
