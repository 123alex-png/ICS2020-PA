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
  // char argv[12][80];
  if(args == NULL) {
    exit(main(0, empty, empty));
  }
  else{
    // for(int i = 1; i <= *args; i++){
    //   printf("args[%d] = %p\n", i, args[i]);
    //   char *p = (char *)args[i];
    //   printf("p = %p\n", p);
    //   int j = 0;
    //   for(j = 0; p[j]!='\0'; j++){
    //     printf("j=%d, p[j]=%c\n", j, p[j]);
    //     argv[i-1][j] = p[j];
    //   }
    //   argv[i-1][j] = '\0';
    // }
    // printf("%p\n", argv);
    // // assert(argv[0]);
    // printf("&argv[0] = %p\n", &argv[0]);
    // printf("argv[0] = %s\n", argv[0]);
    // exit(main(*args, (char **)argv, empty));
    char **argv = (char **)(args + 1);
    printf("argv[0] = %p\n", ((char **)argv)[0]);
    exit(main(*args, (char **)argv, empty));
  }
  assert(0);
}
