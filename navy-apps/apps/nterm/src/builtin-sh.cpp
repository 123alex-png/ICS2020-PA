#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);
const char *items[] = {
  {"nterm"},
  {"nslider"},
  {"fceux"},
  {"fceux"},
  {"bird"},
  {"pal"},
  {"nplayer"},
  {"coremark"},
  {"dhrystone"},
  {"typing-game"},
  {"onscripter"},
};
static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  const char *split = " ";
  char *str = (char *)malloc(strlen(cmd));
  for(int i = 0; cmd[i]!='\0';i++){
    str[i] = cmd[i];
  }
  for(int i = 0;i < 11; i++){
    char *item = strtok(str, split);
    if(!strncmp(item, items[i], strlen(cmd)-1)){
      char **args =(char **)malloc(sizeof(char **) * 10);
      args[0] = str + strlen(str) + 1;
      if(args[0][strlen(args[0])-2]>0x1f){//x86-nemu没有'\t'，故需特判
        args[0][strlen(args[0])-1]='\0';
      }
      else{
        args[0][strlen(args[0])-2]='\0';
      }
      args[1] = NULL;
      execvp(items[i], (char* const*)args);
    }
  }
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();
  setenv("PATH", "/bin", 0);
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
