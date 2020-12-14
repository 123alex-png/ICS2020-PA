#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);
struct MenuItem {
  const char *name, *bin, *arg1;
} items[] = {
  {"nterm\n", "/bin/nterm", NULL},
  {"nslider\n", "/bin/nslider", NULL},
  {"FCEUX (Super Mario Bros)\n", "/bin/fceux", "/share/games/nes/mario.nes"},
  {"FCEUX (100 in 1)\n", "/bin/fceux", "/share/games/nes/100in1.nes"},
  {"bird\n", "/bin/bird", NULL},
  {"pal\n", "/bin/pal", NULL},
  {"NPlayer\n", "/bin/nplayer", NULL},
  {"coremark\n", "/bin/coremark", NULL},
  {"dhrystone\n", "/bin/dhrystone", NULL},
  {"typing-game\n", "/bin/typing-game", NULL},
  {"ONScripter\n", "/bin/onscripter", NULL},
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
  for(int i = 0;i < 11; i++){
    if(!strncmp(cmd, items[i].bin, strlen(cmd)-1)){
      const char *exec_argv[3];
      exec_argv[0] = items[i].bin;
      exec_argv[1] = items[i].arg1;
      exec_argv[2] = NULL;
      execvp(exec_argv[0], (char**)exec_argv);
    }
  }
}

extern char **environ;
void builtin_sh_run() {
  sh_banner();
  sh_prompt();
  printf("%s\n", environ[0]);
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
