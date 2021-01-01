#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {//后2个参数的用途？？？
  printf("vaddr=%x\n",vaddr);
  assert(0);
  uintptr_t pgdir = paddr_read(PTE_ADDR(cpu.cr3)+PDE_INDEX(vaddr)*4, 4);
  assert((pgdir & PTE_P)==1);
  uintptr_t pgtab = paddr_read(PTE_ADDR(pgdir)+PTE_INDEX(vaddr)*4, 4 );
  assert(pgtab & PTE_P);
  paddr_t paddr = PTE_ADDR(pgtab)+OFFSET(vaddr);
  assert(paddr==vaddr);
  // while(1);
  return paddr;
}

word_t vaddr_mmu_read(vaddr_t addr, int len, int type) {
  if (OFFSET(addr)+len <= PAGE_SIZE) {
    paddr_t pg_base = isa_mmu_translate(addr, type, len);
    return paddr_read(pg_base, len);
  } else {
    assert(OFFSET(addr)+len <= PAGE_SIZE);
  }
  assert(0);
  return 0;
}

void vaddr_mmu_write(vaddr_t addr, uint32_t data, int len, int type){
  if (OFFSET(addr)+len <= PAGE_SIZE) {
    paddr_t pg_base = isa_mmu_translate(addr, type, len);
    return paddr_write(pg_base, data, len);
  } else {
    assert(OFFSET(addr)+len <= PAGE_SIZE);
  }
  assert(0);
}