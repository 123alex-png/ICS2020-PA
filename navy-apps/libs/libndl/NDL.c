#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

int _gettimeofday(struct timeval *tv, struct timezone *tz);
int _open(const char *path, int flags, mode_t mode);
int _read(int fd, void *buf, size_t count);

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint32_t ret = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  return ret;
}

int NDL_PollEvent(char *buf, int len) {
    int fd=_open("/dev/events", 0, 0);
    int ret=_read(fd, buf, len);
    if(ret!=0){
      return 1;
    }
return 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  int fd = _open("/proc/dispinfo", 0, 0);
  char *buf[21];
  _read(fd, buf, 20);
  char num1[10],num2[10];
  strncpy(num1, buf, 10);
  strncpy(num2, buf + 10, 10);
  int width = atoi(num1), height = atoi(num2);
  if (*w == 0 && *h == 0 ){
    screen_h = height;
    screen_w = width;
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
  return 0;
}

void NDL_Quit() {
}
