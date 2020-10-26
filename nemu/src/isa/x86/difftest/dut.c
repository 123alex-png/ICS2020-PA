#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if(ref_r->eax!=cpu.eax)return false;
  if(ref_r->edx!=cpu.edx)return false;
  if(ref_r->ecx!=cpu.ecx)return false;
  if(ref_r->ebx!=cpu.ebx)return false;
  if(ref_r->esp!=cpu.esp)return false;
  if(ref_r->ebp!=cpu.ebp)return false;
  if(ref_r->esi!=cpu.esi)return false;
  if(ref_r->edi!=cpu.edi)return false;
  if(ref_r->eflags.CF!=cpu.eflags.CF)return false;
  if(ref_r->pc!=cpu.pc)return false;
  return true;
}

void isa_difftest_attach() {
}
