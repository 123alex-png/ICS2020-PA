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

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

#define FT_SIZE (sizeof(file_table)/20)

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);


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
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

static size_t open_offset[100];

void init_fs() {
  // TODO: initialize the size of /dev/fb
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

size_t fs_read(int fd, void *buf, size_t len){
  size_t offset=file_table[fd].disk_offset;
  size_t size=file_table[fd].size;
  size_t real_len=len;
  if(open_offset[fd]+len>size){
    real_len=size-len;
  }
  open_offset[fd]+=real_len;
  return ramdisk_read(buf,offset+open_offset[fd],real_len);
}

size_t fs_write(int fd, const void *buf, size_t len){
  size_t offset=file_table[fd].disk_offset;
  return ramdisk_write(buf,offset,len);
}

off_t lseek(int fd, off_t offset, int whence){
  switch(whence){
    case SEEK_SET:{
      file_table[fd].disk_offset=offset;
      break;
    }
    case SEEK_CUR:{
      file_table[fd].disk_offset+=offset;
      break;
    }
    case SEEK_END:{
      file_table[fd].disk_offset=file_table[fd].size+offset;
      break;
    }
    default:
      assert(0);
  }
  return file_table[fd].disk_offset;
}