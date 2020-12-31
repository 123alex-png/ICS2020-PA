#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_KEYBRD, FD_DISPINFO};

#define FT_SIZE (sizeof(file_table)/sizeof(Finfo))

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

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
  [FD_FB]     = {"/dev/fb", 0, 0, invalid_read, fb_write},
  [FD_KEYBRD] = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

// static size_t open_offset[100];

void init_fs() {
  // TODO: initialize the size of /dev/fb
  int width = io_read(AM_GPU_CONFIG).width, height = io_read(AM_GPU_CONFIG).height;
  // printf("%d %d\n", width, height);
  file_table[FD_FB].size = width * height;
}

int fs_open(const char *pathname, int flags, int mode){
  for(int i=0;i<FT_SIZE;i++){
    //if(file_table[i].name)
    {
      if(!strcmp(pathname,file_table[i].name)){
        file_table[i].open_offset = 0;
        return i;
      }
    }
  }
  assert(0);
  return -1;
}

int fs_close(int fd){
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len){
  // printf("%s\n", file_table[fd].name);
  if(file_table[fd].read != NULL){
    size_t ret = file_table[fd].read(buf, file_table[fd].open_offset+file_table[fd].disk_offset, len);
    file_table[fd].open_offset += ret;
    return ret;
  }
  if(file_table[fd].open_offset >= file_table[fd].size){
    return 0;
  }
  if(file_table[fd].open_offset+len>file_table[fd].size){
    len=file_table[fd].size - file_table[fd].open_offset;
  }
  ramdisk_read(buf, file_table[fd].disk_offset+file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len){
  // size_t offset=file_table[fd].disk_offset+file_table[fd].open_offset;
  // file_table[fd].open_offset+=len;
  // if(file_table[fd].write==NULL)file_table[fd].write=ramdisk_write;
  // return file_table[fd].write(buf,offset,len);

    size_t ret = 0;
  if(file_table[fd].write){
    ret = file_table[fd].write(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += ret - 768;
    return ret;
  }
  if(file_table[fd].open_offset>=file_table[fd].size)return 0;
  size_t real_len=file_table[fd].size - file_table[fd].open_offset;
  if(len > real_len)len = real_len;
  ret = ramdisk_write(buf ,file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += ret;
  return ret;
}


size_t fs_lseek(int fd, size_t offset, int whence){
  switch(whence){
    case SEEK_SET:{
      // if(offset>file_table[fd].size){
      //   //assert(0);
      //   return (off_t)-1;
      // }
      file_table[fd].open_offset=offset;
      //printf("cur=%d,offset=%d,whence = %d, size = %d\n",file_table[fd].open_offset,offset,whence,file_table[fd].size);
      break;
    }
    case SEEK_CUR:{
      // if(file_table[fd].open_offset+offset>file_table[fd].size){
      //   //assert(0);
      //   return (off_t)-1;
      // }
      
      file_table[fd].open_offset+=offset;
      break;
    }
    case SEEK_END:{
      // if(offset>0){
      //   //assert(0);
      //   return (off_t)-1;
      // }
      file_table[fd].open_offset=file_table[fd].size+offset;
      break;
    }
    default:
      return -1;
  }
  return file_table[fd].open_offset;
}
