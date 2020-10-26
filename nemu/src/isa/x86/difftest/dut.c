#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"
#include <stdlib.h>

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool flag=true;
  int type;
  if(ref_r->eax!=cpu.eax){flag=false;type=0;}
  if(ref_r->ecx!=cpu.ecx){flag=false;type=1;}
  if(ref_r->edx!=cpu.edx){flag=false;type=2;}
  if(ref_r->ebx!=cpu.ebx){flag=false;type=3;}
  if(ref_r->esp!=cpu.esp){flag=false;type=4;}
  if(ref_r->ebp!=cpu.ebp){flag=false;type=5;}
  if(ref_r->esi!=cpu.esi){flag=false;type=6;}
  if(ref_r->edi!=cpu.edi){flag=false;type=7;}
  if(!flag){
    char *name=(char*)malloc(sizeof(char));
    rtlreg_t ref,dut;
    switch (type)
    {
    case 0:
      name="eax";
      ref=ref_r->eax;
      dut=cpu.eax;
      break;
    case 1:
      name="ecx";
      ref=ref_r->ecx;
      dut=cpu.ecx;
      break;
    case 2:
      name="edx";
      ref=ref_r->edx;
      dut=cpu.edx;
      break;
    case 3:
      name="ebx";
      ref=ref_r->ebx;
      dut=cpu.ebx;
      break;
    case 4:
      name="esp";
      ref=ref_r->esp;
      dut=cpu.esp;
      break;
    case 5:
      name="ebp";
      ref=ref_r->ebp;
      dut=cpu.ebp;
      break;
    case 6:
      name="esi";
      ref=ref_r->esi;
      dut=cpu.esi;
      break;
    case 7:
      name="edi";
      ref=ref_r->edi;
      dut=cpu.edi;
      break;
    default:
      break;
    }
    difftest_check_reg(name,pc,ref,dut);
    return false;
  }
  return true;
}

void isa_difftest_attach() {
}
