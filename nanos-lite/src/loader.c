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
#define stdprot (0XFFFFFFFF)
#define min(x, y) (x < y ? x: y)


void page_load(int fd, PCB *pcb, uintptr_t vaddr, uint32_t filesz, uint32_t memsz){
  // printf("vaddr = %p, filesz = %p, end = %p, memsz = %p, real_end = %p\n", vaddr, filesz, vaddr + filesz, memsz, vaddr + memsz);
  uintptr_t align_vaddr = vaddr;
  if(vaddr%PGSIZE!=0){
    align_vaddr = ROUNDDOWN(vaddr, PGSIZE);
  }
    void *pa = 0;
  // printf("align_vaddr: %p\n", align_vaddr);
  pa = new_page(1);
  map(&(pcb->as), (void *)align_vaddr, pa, stdprot);    
  int left = min(filesz, PGSIZE - vaddr + align_vaddr);
  fs_read(fd, (void *)(pa + vaddr - align_vaddr), left);
  int tmp = vaddr - align_vaddr + left;
  memset(pa + tmp, 0, min(memsz - filesz, PGSIZE - tmp));

  filesz -= left;
  memsz -= left;
  vaddr = align_vaddr + PGSIZE;
  assert((int)filesz >= 0);
  // assert(0);
  int i;
  // printf("filesz: %p\n", filesz);
  for(i = 0; i < (int)(filesz); i+=PGSIZE){//如果文件大小8K+1，则i最大遍历到1，读完后还有1字节未处理，这种情况几乎一定发生
    // printf("%d\n", i);
    void *paddr = 0;
    // printf("vaddr + i: %p\n", vaddr+i);
    paddr = new_page(1);
    map(&(pcb->as), (void *)(vaddr+i), paddr, stdprot); 
    fs_read(fd, (void *)paddr, min(PGSIZE, filesz - i));
    if(PGSIZE > filesz - i){
      memset(paddr + filesz - i, 0, PGSIZE - (filesz - i));
    }
  }
  while(i < (int)memsz){
    void *paddr = 0;
    paddr = new_page(1);
    map(&(pcb->as), (void *)(vaddr+i), paddr, stdprot); 
    memset(paddr, 0, min(PGSIZE, memsz - i));
    i += PGSIZE;
  }
}

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  // char fname[80];
  // int j;
  // for(j = 0; filename[j] != '\0'; j++){
  //   if(filename[j] == 1){
  //     break;
  //   }
  //   else{
  //     fname[j] = filename[j];
  //   }
  // }
  // fname[j] = '\0';
  // printf("filename: %s, fname: %s\n", filename, fname);
  int fd=fs_open(filename,0,0);
  fs_read(fd,&ehdr,sizeof(ehdr));
  uint16_t phnum=ehdr.e_phnum;
  for(int i=0;i<phnum;i++){
      fs_lseek(fd,ehdr.e_phoff+i*sizeof(phdr),SEEK_SET);
      fs_read(fd,&phdr,ehdr.e_phentsize);
    if(phdr.p_type==PT_LOAD){
      fs_lseek(fd,phdr.p_offset,SEEK_SET);
      page_load(fd, pcb, phdr.p_vaddr, phdr.p_filesz, phdr.p_memsz);//以页为单位加载
    }
  }
  return ehdr.e_entry;
}

void context_kload(PCB *pcb, void *entry, void *arg){
  Area kstack;
  kstack.start = pcb->stack;
  kstack.end = kstack.start + sizeof(pcb->stack);
  pcb->cp = kcontext(kstack, entry, arg);
}
int cnt = 0;
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  // printf("filename:%s\n", filename);
  protect(&(pcb->as));
  // char *tmp[80];
  // int i;
  // for(i = 0; argv[i]!=NULL;i++){
  //   int j;
  //   for(j = 0; argv[i][j]!='\0';j++){
  //     tmp[i][j] = argv[i][j];
  //   }
  //   tmp[i][j] = '\0';
  // }
  // tmp[i] = (char *)NULL;
  
  volatile uintptr_t entry = loader(pcb, filename);
  printf("app/test entry: %p\n", entry);
  Area kstack;
  kstack.start = pcb->stack;
  kstack.end = kstack.start + sizeof(pcb->stack);
  
  pcb->cp = ucontext(&(pcb->as), kstack, (void *)entry);
  
  Area ustack;
  ustack.start = new_page(8);
  ustack.end = ustack.start + sizeof(pcb->stack);
  Context *c = pcb->cp;
  if(argv != NULL){
    void *ptr[12];
    char *argp = (char *)ustack.end;
    int argc = 0;
    for(int i = 0; argv[i] != NULL; i++){
      *argp-- = '\0';
      if(argv[i][0]!='0'){
        for(int j = strlen(argv[i])-1; j >= 0; j--){
          *argp-- = argv[i][j];
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
  }
  else{
    c -> GPRx = 0;
  }
}