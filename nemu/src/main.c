#include "monitor/debug/expr.h"
#include<stdlib.h>
void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);
  //begin test
  FILE *fp;
  fp=fopen("tools/gen-expr/input","r");
  char buf[65536];
  while(fgets(buf,65536,fp)!=NULL){
    char *arg1=strtok(buf," ");
    bool *success1=(bool*)malloc(sizeof(bool));
    *success1=true;
    bool *flag1=(bool*)malloc(sizeof(bool));
    *flag1=true;
    unsigned int ans1=expr(buf,success1,flag1);
    if(atoi(arg1)==ans1){
      printf("correct! %u\n",ans1);
    }
    else{
      printf("wrong! the answer is %u, gets %u",atoi(arg1),ans1);
    }
  }
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
