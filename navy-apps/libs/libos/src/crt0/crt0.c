#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  char *argv[12];
  for(int i = 0; i < 12; i++){
    argv[i] = (char *)malloc(12 * 80);
  }
  char *p = (char *)args;
  for(int i = 0; i < *args; i++){
    int j = 0;
    for(j = 0; p[j]!='\0'; j++){
      argv[i][j] = p[j];
    }
    argv[i][j] = '\0';
    p += j;
  }
  char *empty[] =  {NULL };
  environ = empty;
  exit(main(0, argv, empty));
  assert(0);
}
