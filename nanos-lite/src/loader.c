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
  Area kstack;
  kstack.start = pcb->stack;
  kstack.end = kstack.start + sizeof(pcb->stack);
  pcb->cp = kcontext(kstack, entry, arg);
  // protect(&(pcb->as));
  // pcb->cp->as = &(pcb->as);
}
int cnt = 0;
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  char *tmp[80];
  for(int i = 0; i < 80; i++){
    tmp[i] = (char *)malloc(sizeof(char) * 80);
  }
  int i;
  for(/*int */i = 0; argv[i]!=NULL;i++){
    int j;
    for(j = 0; argv[i][j]!='\0';j++){
      tmp[i][j] = argv[i][j];
    }
    tmp[i][j] = '\0';
  }
  tmp[i] = (char *)NULL;

  uintptr_t entry = loader(pcb, filename);
  Area kstack;
  kstack.start = pcb->stack;
  printf("start = %p\n", kstack.start);
  kstack.end = kstack.start + sizeof(pcb->stack);
  
  pcb->cp = ucontext(&(pcb->as), kstack, (void *)entry);
  
  Area ustack;
  ustack.start = new_page(8);
  ustack.end = ustack.start + sizeof(pcb->stack);
  Context *c = pcb->cp;
  if(tmp != NULL){
    void *ptr[12];
    char *argp = (char *)ustack.end;
    int argc = 0;
    for(int i = 0; tmp[i] != NULL; i++){
      *argp-- = '\0';
      if(tmp[i][0]!='0'){
        for(int j = strlen(tmp[i])-1; j >= 0; j--){
          *argp-- = tmp[i][j];
        }
      }
      ptr[i] = argp + 1;
      argc++;
    }
    uintptr_t tmp = ((uintptr_t)argp >>3) <<3;
    uintptr_t *p = (void *)(tmp) - (argc + 2)*sizeof(uintptr_t);
    for(int i = 0; i < argc; i++){
      p[i+1] = (uintptr_t)ptr[i]; 
    }
    *p = argc;
    c -> GPRx = (uintptr_t)p;
    printf("c->GPRx = %p\n", c->GPRx);
  }
  else{
    c -> GPRx = 0;
  }
}