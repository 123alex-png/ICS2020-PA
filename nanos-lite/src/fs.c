#include <fs.h>
#include <sys/types.h>
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_KEYBRD};

#define FT_SIZE (sizeof(file_table)/sizeof(Finfo))

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_KEYBRD] = {"/dev/events", 0, 0, events_read, invalid_write},
#include "files.h"
};

static size_t open_offset[100];

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

size_t getoffset(int fd){
  return file_table[fd].disk_offset;
}

int fs_open(const char *pathname, int flags, int mode){
  for(int i=3;i<FT_SIZE;i++){
    if(!strcmp(pathname,file_table[i].name)){
      return i;
    }
  }
  assert(0);
  return -1;
}

int fs_close(int fd){
  return 0;
}

size_t off;
size_t fs_read(int fd, void *buf, size_t len){
  off=file_table[fd].disk_offset;
  size_t size=file_table[fd].size;
  size_t real_len=len;
  if(open_offset[fd]+len>size){
    real_len=size-open_offset[fd];
  }
  if(file_table[fd].read==NULL){
    file_table[fd].read=ramdisk_read;
  }
  else{
    real_len = len;
  }
  file_table[fd].read(buf,off+open_offset[fd],real_len);
  open_offset[fd]+=real_len;
  return real_len;
}

size_t fs_write(int fd, const void *buf, size_t len){
  size_t offset=file_table[fd].disk_offset+open_offset[fd];
  open_offset[fd]+=len;
  if(file_table[fd].write==NULL)file_table[fd].write=ramdisk_write;
  return file_table[fd].write(buf,offset,len);
}


off_t fs_lseek(int fd, off_t offset, int whence){
  
  switch(whence){
    case SEEK_SET:{
      if(offset>file_table[fd].size){
        //assert(0);
        return (off_t)-1;
      }
      open_offset[fd]=offset;
      //printf("cur=%d,offset=%d,whence = %d, size = %d\n",open_offset[fd],offset,whence,file_table[fd].size);
      break;
    }
    case SEEK_CUR:{
      if(open_offset[fd]+offset>file_table[fd].size){
        //assert(0);
        return (off_t)-1;
      }
      open_offset[fd]+=offset;
      break;
    }
    case SEEK_END:{
      if(offset>0){
        //assert(0);
        return (off_t)-1;
      }
      open_offset[fd]=file_table[fd].size+offset;
      break;
    }
    default:
      assert(0);
  }
  return open_offset[fd];
}