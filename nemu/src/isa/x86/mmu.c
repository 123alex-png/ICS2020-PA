#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {//后2个参数的用途？？？
  // printf("vaddr=%x\n",vaddr);
  // Log("PTE_ADDR(cpu.cr3)+PDE_INDEX(vaddr)*4: %x\n",PTE_ADDR(cpu.cr3)+PDE_INDEX(vaddr)*4);
  // assert(0);
  uintptr_t pgdir = paddr_read(PTE_ADDR(cpu.cr3)+PDE_INDEX(vaddr)*4, 4);
  // printf("pgdir: %x\n",(uint32_t)pgdir);
  // assert((pgdir & PTE_P)==1);
  uintptr_t pgtab = paddr_read(PTE_ADDR(pgdir)+PTE_INDEX(vaddr)*4, 4 );
  // assert(pgtab & PTE_P);
  paddr_t paddr = PTE_ADDR(pgtab)+OFFSET(vaddr);
  assert(paddr==vaddr);
  // while(1);
  return paddr;
}

word_t vaddr_mmu_read(vaddr_t addr, int len, int type) {
  // printf("addr: %x\n",addr);
  if (OFFSET(addr)+len <= PAGE_SIZE) {
    paddr_t pg_base = isa_mmu_translate(addr, type, len);
    assert(pg_base==addr);
    return paddr_read(addr, len);
  } else {
    assert(OFFSET(addr)+len <= PAGE_SIZE);
  }
  assert(0);
  return 0;
}

void vaddr_mmu_write(vaddr_t addr, uint32_t data, int len, int type){
  paddr_write(addr, data, len);return;
  if (OFFSET(addr)+len <= PAGE_SIZE) {
    paddr_t pg_base = isa_mmu_translate(addr, type, len);
    paddr_write(pg_base, data, len);
    return;
  } else {
    assert(OFFSET(addr)+len <= PAGE_SIZE);
  }
  assert(0);
}