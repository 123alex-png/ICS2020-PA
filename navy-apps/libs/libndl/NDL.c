#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

int _gettimeofday(struct timeval *tv, struct timezone *tz);
int _open(const char *path, int flags, mode_t mode);
int _read(int fd, void *buf, size_t count);
int _write(int fd, void *buf, size_t count);
off_t _lseek(int fd, off_t offset, int whence);

FILE *fp1;



uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint32_t ret = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  return ret;
}

int NDL_PollEvent(char *buf, int len) {
    // int fd=_open("/dev/events", 0, 0);
    // int ret=_read(fd, buf, len);
    // if(ret!=0){
    //   return 1;
    // }
    fp1=fopen("/dev/events", "r");
    int ret=fread(buf, 1, len, fp1);
    // fscanf(fp,"%s", buf+3);
    // fscanf(fp, "%s", buf);
    if(ret){
      return 1;
    }
    return 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  //int fd = _open("/proc/dispinfo", 0, 0);
  FILE *fp = fopen("/proc/dispinfo", "r");
  char buf[50];
  //_read(fd, buf, 50);
  fread(buf, 1, 50, fp);
  size_t info = atoi(buf);
  int width = info >> 16, height = info & 0xffff;
  screen_h = height;
  screen_w = width;
  //printf("%d %d\n",screen_w, screen_h);
  if (*w == 0 && *h == 0 ){
    *h = height;
    *w = width;
  }
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int fd = _open("/dev/fb", 0, 0);

  // size_t len = (w << 16) + 1;
  // for (int j = 0; j < h && y + j < screen_h; j ++) {
  //     size_t offset = (y + j) * screen_w  + x;
  //     lseek(fd, offset, SEEK_SET);
  //     _write(fd, pixels, len);
  //     pixels += w;
  // }
  size_t len = (w << 16) + h;
      size_t offset = (y) * screen_w  + x;
      lseek(fd, offset, SEEK_SET);
      _write(fd, pixels, len);
      pixels += w;
  
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  // fp1=fopen("/dev/events", "r");
  return 0;
}

void NDL_Quit() {
}
