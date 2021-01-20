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
#define round_down(addr) ((addr & PGSIZE) | (addr & ~(PGSIZE - 1)))
#define stdprot (0XFFFFFFFF)
void page_load(int fd, PCB *pcb, uintptr_t vaddr, uint32_t filesz, uint32_t memsz){
  // printf("vaddr = %p\n", vaddr);
  uintptr_t align_vaddr = vaddr;
  if(vaddr%PGSIZE!=0){
    align_vaddr = round_down(vaddr);
  }
  {
    void *paddr = new_page(1);
    map(&(pcb->as), (void *)align_vaddr, paddr, stdprot);
    fs_read(fd, (void *)(paddr + vaddr - align_vaddr), PGSIZE - vaddr + align_vaddr);
    memset(paddr, 0, vaddr - align_vaddr);
  }
  filesz -= vaddr - align_vaddr;
  memsz -= vaddr - align_vaddr;
  vaddr = align_vaddr + PGSIZE;
  assert(filesz >= 0);
  // assert(0);
  size_t i;
  for(i = 0; i < filesz/PGSIZE; i++){//如果文件大小8K+1，则i最大遍历到1，读完后还有1字节未处理，这种情况几乎一定发生
    // printf("%d\n", i);
    void *paddr = new_page(1);
    map(&(pcb->as), (void *)(vaddr+i*PGSIZE), paddr, stdprot);
    fs_read(fd, (void *)paddr, PGSIZE);
  }
  //处理filesz剩余部分
  int leftsz = filesz - i * PGSIZE;
  assert(leftsz >= 0);
  
  void *paddr = new_page(1);
  map(&(pcb->as), (void *)(vaddr+i*PGSIZE), paddr, stdprot);
  fs_read(fd, (void *)paddr, leftsz);
  // memset((void *)(paddr+leftsz), 0, PGSIZE-leftsz);//该页剩余部分填0
  //清零memsz-filesz
  // int left = memsz - filesz - (PGSIZE - leftsz);
  // uintptr_t start = vaddr+(i+1)*PGSIZE;
  // size_t j;
  // for(j=0; j < left/PGSIZE; j++){
  //   printf("j=%d\n", j);
  //   uintptr_t paddr = (uintptr_t)new_page(1);
  //   map(&(pcb->as), (void *)(start+j*PGSIZE), (void *)paddr, 0);
  //   memset((void *)paddr, 0, PGSIZE);
  // }
  // //清零还剩一点
  // int left_memsz = left - j * PGSIZE;
  // uintptr_t paddr1 = (uintptr_t)new_page(1);
  // map(&(pcb->as), (void *)(start+j*PGSIZE), (void *)paddr1, 0);
  // memset((void *)paddr1, 0, left_memsz);
}

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
      
      // page_load(fd, pcb, phdr.p_vaddr, phdr.p_filesz, phdr.p_memsz);//以页为单位加载
    }
  }
  return ehdr.e_entry;
}

// static uintptr_t loader(PCB *pcb, const char *filename) {
//   //TODO();
  
//   int fd=fs_open(filename,0,0);
//   fs_read(fd,&ehdr,sizeof(ehdr));
//   uint16_t phnum=ehdr.e_phnum;
//   for(int i=0;i<phnum;i++){
//       fs_lseek(fd,ehdr.e_phoff+i*sizeof(phdr),SEEK_SET);
//       fs_read(fd,&phdr,ehdr.e_phentsize);
//     if(phdr.p_type==PT_LOAD){
//       fs_lseek(fd,phdr.p_offset,SEEK_SET);
//       // fs_read(fd,(void *)phdr.p_vaddr,phdr.p_filesz);
//       // memset((void *)(phdr.p_vaddr+phdr.p_filesz),0,phdr.p_memsz-phdr.p_filesz);
//       page_load(fd, pcb, phdr.p_vaddr, phdr.p_filesz, phdr.p_memsz);//以页为单位加载
//     }
//   }
//   return ehdr.e_entry;
// }


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
  protect(&(pcb->as));
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
  // printf("start = %p\n", kstack.start);
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
    // printf("c->GPRx = %p\n", c->GPRx);
  }
  else{
    c -> GPRx = 0;
  }
}