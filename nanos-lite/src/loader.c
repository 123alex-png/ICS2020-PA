#include <proc.h>
#include <elf.h>
#include <fs.h>

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


int fs_open(const char *pathname, int flags, int mode);
int fs_close(int fd);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
Elf_Ehdr ehdr;
Elf_Phdr phdr;
static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  
  int fd=fs_open(filename,0,0);
  fs_read(fd,&ehdr,sizeof(ehdr));
  uint16_t phnum=ehdr.e_phnum;
  for(int i=0;i<phnum;i++){
      fs_lseek(fd,ehdr.e_phoff+i*sizeof(phdr),SEEK_SET);
      fs_read(fd,&phdr,ehdr.e_phentsize);
    if(phdr.p_type==PT_LOAD){
      fs_lseek(fd,phdr.p_offset,SEEK_SET);
      fs_read(fd,(void *)phdr.p_vaddr,phdr.p_filesz);
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

void context_kload(PCB *pcb, void *entry, void *arg){
  Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);
  pcb->cp = kcontext(stack, entry, arg);
  // protect(&(pcb->as));
  // pcb->cp->as = &(pcb->as);
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  uintptr_t entry = loader(pcb, filename);
  Area ustack;
  ustack.start = new_page(8);
  // ustack.start = pcb->stack;
  ustack.end = ustack.start + sizeof(pcb->stack);
  
  pcb->cp = ucontext(&(pcb->as), ustack, (void *)entry);
  Context *c = pcb->cp;
  if(argv != NULL){
    intptr_t *argp = ustack.end - sizeof(pcb) - 0x2000;
    // printf("argp = %p\n", argp);
    int argc = 1;
    char *last= (char *)argp + 0x100;
    for(; /*argv[argc]!=NULL*/argv[argc-1]!=NULL; argc++){
      argp[argc] = (intptr_t)last;
      // printf("argp[%d] = %p\n", argc, argp[argc]);
      last += strlen(argv[argc-1]);
  }
    argp[argc] = 0;
    --argc;
    printf("argc = %d", argc);
    argp[0] = argc;
    c -> GPRx = (uintptr_t)argp;
    char *end = (char *)argp + 0x100;//至多可放12个参数，所有参数长度和至多80字节
    for(int i = 0; i < argc; i++){
      for(int j = 0; j < strlen(argv[i]); j++){
        *end++ = argv[i][j];
        // printf("end = %p, *end = %p\n", end - 1, *(end-1)&0xff);
      }
      *end++ = '\0';
    }
  }
  else{
    c -> GPRx = 0;
  }
}