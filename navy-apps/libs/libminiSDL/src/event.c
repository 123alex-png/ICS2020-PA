#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>
#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];
  ev->type = SDL_KEYUP;
  if(NDL_PollEvent(buf, sizeof(buf))){
    printf("receive event: %s\n", buf);
    if(buf[1]=='u'){
      ev->type = SDL_KEYUP;
    }
    else{
      ev->type = SDL_KEYDOWN;
    }
    char *key = buf+3;
    //printf("key = %s, %d\n", key, strlen(key));
    for(int i = 0; i < 83; i++){
      //printf("%s, %d\n", keyname[i], strlen(keyname[i]));
      if(!strcmp(key, keyname[i])){
        ev->key.keysym.sym = i;
        return 0;
      }
    }
  }
  return -1;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64];
  event->type = SDL_KEYUP;
  if(NDL_PollEvent(buf, sizeof(buf))){
    printf("receive event: %s\n", buf);
    if(buf[1]=='u'){
      event->type = SDL_KEYUP;
    }
    else{
      event->type = SDL_KEYDOWN;
    }
    char *key = buf+3;
    //printf("key = %s, %d\n", key, strlen(key));
    for(int i=1; i < 83; i++){
      //printf("%s, %d\n", keyname[i], strlen(keyname[i]));
      if(!strcmp(key, keyname[i])){
        event->key.keysym.sym = i;
        return 0;
      }
    }
  }
  return -1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
