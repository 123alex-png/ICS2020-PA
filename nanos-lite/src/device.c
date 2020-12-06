#include <common.h>
#include <assert.h>
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
  for(size_t i=0;i<len;i++){
        putch(*(char *)(buf+i));
      }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev;
  ioe_read(AM_INPUT_KEYBRD, &ev);
  if(ev.keycode == AM_KEY_NONE){
    return 0;
  }
  char event[50];
  sprintf("Got event: %s  %s\n",names[ev.keycode], ev.keydown ? "DOWN" : "UP");
  strncpy(buf, event, len);
  size_t ret = strlen(buf);
  // if (ret != strlen(event)){
  //   assert(0);
  // }
  return ret;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
