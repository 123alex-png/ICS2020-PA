#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>
#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t keystate[83];

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  if(ev == NULL){
    return 1;
  }
  char buf[64];
  ev->type = SDL_KEYUP;
  if(NDL_PollEvent(buf, sizeof(buf))){
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
        //printf("receive event: %s\n", buf);
        ev->key.keysym.sym = i;
        if(ev->type==SDL_KEYUP){
          keystate[i] = 0;
        }
        else{
          keystate[i] = 1;
        }
        return 1;
      }
    }
  }
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64];
  event->type = SDL_KEYUP;
  if(NDL_PollEvent(buf, sizeof(buf))){
    //printf("receive event: %s\n", buf);
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
        return 1;
      }
    }
  }
  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return keystate;
}
