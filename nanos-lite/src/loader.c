#include <proc.h>
#include <elf.h>
#include <fs.h>
#include <sys/types.h>

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
static Elf_Ehdr ehdr;
static Elf_Phdr phdr;

int fs_open(const char *pathname, int flags, int mode);
int fs_close(int fd);
size_t fs_read(int fd, void *buf, size_t len);
off_t fs_lseek(int fd, off_t offset, int whence);
size_t getoffset(int fd);

// static uintptr_t loader(PCB *pcb, const char *filename) {
//   //TODO();
//   int fd=fs_open(filename,0,0);
//   fs_read(fd,&ehdr,sizeof(ehdr));
//   //ramdisk_read(&ehdr, 0, 64);
//   uint16_t phnum=ehdr.e_phnum;
//   for(int i=0;i<phnum;i++){
//       fs_read(fd,&phdr,ehdr.e_phentsize);
//   //  ramdisk_read(&phdr,ehdr.e_phoff+i*ehdr.e_phentsize,ehdr.e_phentsize);
//     if(phdr.p_type==PT_LOAD){
//       size_t offset=getoffset(fd);//=fs_lseek(fd,0,SEEK_CUR);
      
//       // memcpy((void *)phdr.p_vaddr,(void *)(&ramdisk_start+offset+phdr.p_offset),phdr.p_filesz);
//       ramdisk_read((void *)phdr.p_vaddr, phdr.p_offset + offset, phdr.p_filesz);
//       memset((void *)(phdr.p_vaddr+phdr.p_filesz),0,phdr.p_memsz-phdr.p_filesz);
//     }
//   }
//   return ehdr.e_entry;
// }

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  int fd=fs_open(filename,0,0);
  fs_read(fd,&ehdr,sizeof(ehdr));
  //ramdisk_read(&ehdr, 0, 64);
  uint16_t phnum=ehdr.e_phnum;
  for(int i=0;i<phnum;i++){
      //fs_read(fd,&phdr,ehdr.e_phentsize);
      // ramdisk_read(&phdr, ehdr.e_phoff + i * sizeof(Elf_Phdr), sizeof(Elf_Phdr));
   ramdisk_read(&phdr,ehdr.e_phoff+i*ehdr.e_phentsize,ehdr.e_phentsize);
    if(phdr.p_type==PT_LOAD){
      size_t offset=getoffset(fd);//=fs_lseek(fd,0,SEEK_CUR);
      
      // memcpy((void *)phdr.p_vaddr,(void *)(&ramdisk_start+offset+phdr.p_offset),phdr.p_filesz);
      ramdisk_read((void *)phdr.p_vaddr, phdr.p_offset + offset, phdr.p_filesz);
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

