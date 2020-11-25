#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
Elf_Ehdr ehdr;
Elf_Phdr phdr;
static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  
  ramdisk_read(&ehdr, 0, 64);
  volatile uint16_t phnum=ehdr.e_phnum;
  for(int i=0;i<phnum;i++){
    
    ramdisk_read(&phdr,ehdr.e_phoff+i*ehdr.e_phentsize,ehdr.e_phentsize);
    if(phdr.p_type==PT_LOAD){
      memcpy((void *)phdr.p_vaddr,(void *)(&ramdisk_start+phdr.p_offset),phdr.p_filesz);
      memset((void *)(phdr.p_vaddr+phdr.p_filesz),0,phdr.p_memsz-phdr.p_filesz);
    }
  }
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

