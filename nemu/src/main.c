#include "monitor/debug/expr.h"
#include<stdlib.h>
void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);
  
   for(int i=1;i<argc;i++){
    bool *success=(bool*)malloc(sizeof(bool));
    *success=true;
    bool *flag=(bool*)malloc(sizeof(bool));
    *flag=true;
    expr(argv[i],success,flag);
  }
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
