#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  printf("%p\n",args);
  char *empty[] =  {NULL };
  environ = empty;
  if(args == NULL) {
    exit(main(0, empty, empty));
  }
  else{
    char **argv = (char **)(args + 1);
    exit(main(*args, (char **)argv, empty));
  }
  assert(0);
}
