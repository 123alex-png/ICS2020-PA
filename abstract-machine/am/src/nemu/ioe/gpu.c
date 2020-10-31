#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
static int W,H;
void __am_gpu_init() {
  //uint32_t t=inl(VGACTL_ADDR);
  //W = (t>>16);  // TODO: get the correct width
  //H = (t&0xffff);  // TODO: get the correct height
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
    outl(SYNC_ADDR, 1);
  }
  else{
    uint32_t t=inl(VGACTL_ADDR);
    W = (t>>16);  // TODO: get the correct width
    H = (t&0xffff);  // TODO: get the correct height
    int w = ctl->w, h = ctl->h, x = ctl->x, y = ctl->y;  
    uint32_t *pixels=ctl->pixels;
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    for (int j = 0; j < h && y + j < H; j ++) {
      for(int k = 0; k < w && x + k < W; k++){
        fb[(y + j) * W  + x + k]=*pixels++;
      }
    }
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
