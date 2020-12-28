#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  printf("args = %p,*args = %d\n", args, *args);
  char *empty[] =  {NULL };
  environ = empty;
  char *argv[12];
  if(args == NULL) {
    exit(main(0, empty, empty));
  }
  else{
    for(int i = 0; i < 12; i++){
      argv[i] = (char *)malloc(12 * 80);
    }
    
    for(int i = 1; i <= *args; i++){
      printf("args[%d] = %p\n", i, args[i]);
      char *p = (char *)args[i];
      printf("p = %p\n", p);
      int j = 0;
      for(j = 0; p[j]!='\0'; j++){
        printf("j=%d, p[j]=%x\n", j, p[j]);
        argv[i][j] = p[j];
      }
      argv[i][j] = '\0';
    }
    // assert(argv[0]);
    // printf("argv[0] = %s\n", argv[0]);
    exit(main(0, argv, empty));
  }
  assert(0);
}
