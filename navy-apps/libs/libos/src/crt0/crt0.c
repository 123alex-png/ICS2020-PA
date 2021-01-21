#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  char *empty[] =  {NULL };
  environ = empty;
  if(args == NULL) {
    exit(main(0, empty, empty));
  }
  else{
    char **argv = (char **)(args + 1);
    // printf("argv = %s\n", argv[0]);
    // printf("argc = %d\n", *(int *)args);
    // if(argv[0])
    // puts(argv[0]);
    exit(main(*args, (char **)argv, empty));
  }
  assert(0);
}
