#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

#define NAMEINIT(key)  [ AM_KEY_##key ] = #key,
static const char *names[] = {
  AM_KEYS(NAMEINIT)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  yield();
  for(size_t i=0;i<len&& *(char *)(buf+i)!='\0';i++){
        putch(*(char *)(buf+i));
      }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  yield();
  AM_INPUT_KEYBRD_T ev;
  ioe_read(AM_INPUT_KEYBRD, &ev);
  if(ev.keycode == AM_KEY_NONE){
    return 0;
  }
  char* upordown=ev.keydown ? "kd" : "ku";
  sprintf(buf, "%s %s", upordown, names[ev.keycode]);
  return len;
}

static int width, height;
size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg;
  ioe_read(AM_GPU_CONFIG, &cfg);
  width = cfg.width;
  height = cfg.height;
  sprintf(buf, "%d %d ", width, height);
  return len;
}

size_t fb_write(void *buf, size_t offset, size_t len) {
      // printf("offset=%d len=%d\n", offset, len);
      // printf("buf :%d\n", strlen(buf));
  void *p = malloc(strlen(buf)+1);
  memcpy(p, buf, strlen(buf));
  printf("buf=%s\n", buf);
  assert(!strcmp(buf, p));
  // yield();
  AM_GPU_FBDRAW_T ctl;
  ctl.x = offset % width;
  ctl.y = offset / width;
  assert(p);
  ctl.pixels = buf;
  ctl.w = len / 4;//len >> 16;
  ctl.h = 1;//len & 0xffff;
  ctl.sync = true;
  ioe_write(AM_GPU_FBDRAW, &ctl);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
