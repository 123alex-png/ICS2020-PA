#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  int t=inl(KBD_ADDR);
  kbd->keycode=t&0xff;
  kbd->keydown=(t>>15)?true:false;
}
