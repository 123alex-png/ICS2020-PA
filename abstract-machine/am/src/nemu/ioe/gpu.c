#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
static int W,H;
void __am_gpu_init() {
  int i;
  uint32_t t=inl(VGACTL_ADDR);
  int W = (t>>16);  // TODO: get the correct width
  int H = (t&0xffff);  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < W * H; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t t=inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = t>>16, .height = t&0xffff,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) {
    int w = ctl->w, h = ctl->h, x = ctl->x, y = ctl->y;  
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    for (int j = 0; j < h && y + j < H; j ++) {
      for(int k = 0; k < w && x + k < W; k++){
        fb[j * H  + k]=*(uint32_t*)(ctl->pixels);
      }
    }
    outl(SYNC_ADDR, 1);
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
