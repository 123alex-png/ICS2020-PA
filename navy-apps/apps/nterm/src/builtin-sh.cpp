#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

#define SIZE_KEY sizeof(items)/sizeof(char *)
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
  {"busybox"}
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
  for(int i = 0;i < SIZE_KEY; i++){
    char *item = strtok(str, split);
    strtok(item, "\n");
    if(!strncmp(item, items[i], strlen(items[i])-1)){
      // assert(0);
      char **args =(char **)malloc(sizeof(char **) * 10);
      memset(args, 0, sizeof(char **) * 10);
      args[0] = str + strlen(str) + 1;
      if(args[0]!=NULL){
        for(int j = 0; args[0][j]!='\0'; j++){
          if(args[0][j]=='\n'||args[0][j]=='\t'){
            args[0][j] = '\0';
            args[1] = NULL;
          }
          if(args[0][j]==' '){
            args[0][j] = '\0';
            printf("%d\n",j);
            args[1] = args[0] + j + 1;
            break;
          }
        }
        if(args[1]!=NULL){
          for(int j = 0; args[1][j]!='\0'; j++){
            if(args[1][j]=='\n'||args[1][j]=='\t'){
              args[1][j] = '\0';
              args[2] = NULL;
            }
          }
        }
      }
      printf("item: %s, args[0]=%s, args[1]:%s\n",item, args[0], args[1]);
      execvp(item, (char* const*)args);
    }
  }
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();
  setenv("PATH", "/bin", 0);
  setenv("PATH", "/usr/bin", 0);
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
