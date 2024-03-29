#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>
uint32_t last_pc=0, last_pgdir=0,last_vaddr;
paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {//后2个参数的用途？？？
  // printf("vaddr=%x\n",vaddr);
  // Log("PTE_ADDR(cpu.cr3)+PDE_INDEX(vaddr)*4: %x",PTE_ADDR(cpu.cr3)+PDE_INDEX(vaddr)*4);
  // assert(0);
  uintptr_t pgdir = paddr_read(PTE_ADDR(cpu.cr3)+PDE_INDEX(vaddr)*4, 4);
  // printf("pgdir: %x\n",(uint32_t)pgdir);
  assert((pgdir & 0xffe) == 0);
  if((pgdir&PTE_P)==0){
    // printf("last_vaddr:%x, lastpc: %x, pc: %x, pgdir: %lx, vaddr: %x, last_pgdir:%x\n", last_vaddr, last_pc, cpu.pc, pgdir,vaddr,last_pgdir);

  }
  last_pc=cpu.pc;
  last_pgdir=pgdir;
  last_vaddr=vaddr;
  assert((pgdir & PTE_P)==1);
  uintptr_t pgtab = paddr_read(PTE_ADDR(pgdir)+PTE_INDEX(vaddr)*4, 4 );
  if(!(pgtab & PTE_P)){printf("pgdir: %lx， ", pgdir);
    printf("pgtab: %lx， vaddr: %x\n", pgtab, vaddr);
  }
  assert(pgtab & PTE_P);//malloc申请的内存占用了pgdir，什么问题？？？
  paddr_t paddr = PTE_ADDR(pgtab)+OFFSET(vaddr);
  // assert(paddr==vaddr);
  // while(1);

  return paddr;
}

word_t vaddr_mmu_read(vaddr_t addr, int len, int type) {
  
  // return paddr_read(addr, len);
  if (OFFSET(addr)+len <= PAGE_SIZE) {
    paddr_t pg_base = isa_mmu_translate(addr, type, len);
    // printf("pg_base : %x\n", pg_base);
    // assert(pg_base==addr);
    return paddr_read(pg_base, len);
  } else {
    size_t len1 = PAGE_SIZE - OFFSET(addr);
    size_t len2 = len - len1;
    
    uintptr_t paddr1 = isa_mmu_translate(addr, type, len);
    uintptr_t paddr2 = isa_mmu_translate(addr+len1, type, len);
    
    word_t ret2 = (paddr_read(paddr2, 4)<<(8*(4-len2)));
    word_t ret1 = ((paddr_read(paddr1, 4))&(len1==1?0xff:(len1==2?0xffff:0xffffff)));
    
    word_t ret = ret1|ret2;
    
    // if(ret != right_data){
    //   printf("addr: %x\n",addr);
    //   printf("len1: %ld, len2: %ld, len: %d\n",len1,len2,len);
    //   printf("paddr1: %lx, paddr2: %lx\n", paddr1,paddr2);
    //   printf("ret1: %x, ret2: %x, read : %x\n", ret1, ret2, ret);
    // printf("ret: %x, right: %x, right: %x\n", ret, right_data,paddr_read(paddr1,len));
    // printf("mask: %x\n",(len1==1?(len1==2?0xffff:0xffffff):0xff));
    // assert(0);
    // }
    return ret;

  }
  assert(0);
  return 0;
}

void vaddr_mmu_write(vaddr_t addr, uint32_t data, int len, int type){
  // paddr_write(addr, data, len);return;
  if (OFFSET(addr)+len <= PAGE_SIZE) {
    paddr_t pg_base = isa_mmu_translate(addr, type, len);
    paddr_write(pg_base, data, len);
    return;
  } else {
    // size_t len1 = PAGE_SIZE - OFFSET(addr);
    // size_t len2 = len - len1;
    // // printf("len1: %ld, len2: %ld\n",len1,len2);
    // paddr_t paddr1 = isa_mmu_translate(addr, type, len);
    // paddr_t paddr2 = isa_mmu_translate(addr+len1, type, len);
    // assert(paddr1==addr&&paddr2==addr+len1);
    // // (paddr_write(paddr2, len2)<<(8*(4-len2)))|((paddr_read(paddr1, 4))>>8*(4-len1));
    // if(len1==2){
    //   paddr_write(paddr2, data>>16, 2);
    //   paddr_write(paddr1, data&0xffff, 2);
    // }
    // else{
    //   if(len1==1){
    //     paddr_write(paddr2, (data>>16)&0xffff, 2);
    //     paddr_write(paddr2+8, data>>24, 1);
    //     paddr_write(paddr1, data&0xff, 1);
    //   }
    //   else{//len1=3
    //     paddr_write(paddr1, data&0xffff, 2);
    //     paddr_write(paddr1+8, (data>>16)&0xff, 1);
    //     paddr_write(paddr2, data>>24, 1);
    //   }
    // }
    // return;
    assert(OFFSET(addr)+len <= PAGE_SIZE);
  }
  assert(0);
}