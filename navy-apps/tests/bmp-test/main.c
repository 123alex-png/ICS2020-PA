#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>

int main() {
  NDL_Init(0);
  int w, h;while(1);
  void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  assert(bmp);
  NDL_OpenCanvas(&w, &h);
  
  printf("%d %d\n",w,h);
  // NDL_DrawRect(bmp, w, h, w, h);
  NDL_DrawRect(bmp, 277, 0, w, h);
  
  free(bmp);
  NDL_Quit();
  
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
