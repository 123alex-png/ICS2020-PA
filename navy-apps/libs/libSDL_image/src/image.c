#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  FILE *fp = fopen(filename, "r");
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  uint8_t *buf = malloc(size);
  fseek(fp, 0, SEEK_SET);
  fread(buf, size, 1, fp);
  assert(buf);
  // for(int i = 0; i < size; i++){
  //   {
  //   printf("buf[%d] = %d\n", i, buf[i]);
  //   if(i>5000)assert(0);}
  // }
  // printf("size = %d\n",size);
  // assert(0);
  SDL_Surface *ret = STBIMG_LoadFromMemory(buf, size);
  assert(ret);
  fclose(fp);
  free(buf);
  return ret;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
