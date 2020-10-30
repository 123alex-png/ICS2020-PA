#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keycode=inl(KBD_ADDR)&0x7fff;
  kbd->keydown=(inl(KBD_ADDR)>>15)?true:false;
}
