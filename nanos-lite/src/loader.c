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
int cnt = 0;
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  // assert(argv[1][1]=='2');
  cnt+=!strcmp(filename, "/bin/exec-test");
  printf("argv[1] = %s", argv[1]);
  printf("cnt=%d\n", cnt);
  if(argv[1][1]!='2')panic("cnt=%d\n", cnt);
  uintptr_t entry = loader(pcb, filename);
  Area ustack;
  ustack.start = new_page(8);
  // ustack.start = pcb->stack;
  ustack.end = ustack.start + sizeof(pcb->stack);
  pcb->cp = ucontext(&(pcb->as), ustack, (void *)entry);
  Context *c = pcb->cp;
  if(argv != NULL){
  //   intptr_t *argp = ustack.end - sizeof(Context) - 0x300;
  //   // printf("argp = %p\n", argp);
  //   int argc = 1;
  //   char *last= (char *)argp + 0x30;
  //   for(; /*argv[argc]!=NULL*/argv[argc-1]!=NULL; argc++){
  //     argp[argc] = (intptr_t)last;
  //     // printf("argp[%d] = %p\n", argc, argp[argc]);
  //     last += strlen(argv[argc-1]);
  // }
  //   argp[argc] = 0;
  //   --argc;
  //   // printf("argc = %d", argc);
  //   argp[0] = argc;
  //   c -> GPRx = (uintptr_t)argp;
  //   char *end = (char *)argp + 0x30;//至多可放12个参数，所有参数长度和至多80字节
  //   for(int i = 0; i < argc; i++){
  //     for(int j = 0; argv[i][j]!='\0'; j++){
  //       *end++ = argv[i][j];
  //       // printf("j = %d, end = %p, *end = %c\n", j, end - 1, *(end-1)&0xff);
  //     }
  //     *end++ = '\0';
  
  //   }
    void *ptr[12];
    char *argp = (char *)ustack.end - sizeof(Context) - 4;
    printf("%p\n", argp);
    int argc = 0;
    for(int i = 0; argv[i] != NULL; i++){
      *argp-- = '\0';
      for(int j = strlen(argv[i])-1; j >= 0; j--){
        *argp-- = argv[i][j];
      }
      ptr[i] = argp + 1;
      printf("at=%p,ptr[%d]=%s\n",ptr[i],i,(char*)ptr[i]);
      argc++;
    }
    printf("%d\n", argc);
    uintptr_t *p = (void *)(argp - argc - 1);
    for(int i = 0; i < argc; i++){
      p[i+1] = (uintptr_t)ptr[argc - i]; 
      printf("at %p, p[] = %p", p+i+1, p[i+1]);
    }
    *p = argc;
    c -> GPRx = (uintptr_t)p;
    while(1);
  }
  else{
    c -> GPRx = 0;
  }
}