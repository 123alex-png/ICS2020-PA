#include "monitor/debug/expr.h"
#include<stdlib.h>
void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);
  /**begin test*/
  // FILE *fp;
  // fp=fopen("tools/gen-expr/input","r");
  // char buf[65536];
  // int cnt=0;
  // while(fgets(buf,65536,fp)!=NULL){
    
  //   char *arg1=strtok(buf," ");
  //   char *arg2=arg1+strlen(arg1)+1;
  //   arg2[strlen(arg2)-1]='\0';
  //   bool *success1=(bool*)malloc(sizeof(bool));
  //   *success1=true;
  //   unsigned int ans1=expr(arg2,success1);
  //   if(atoi(arg1)==ans1){
  //     printf("%d:correct! %u\n",cnt++,ans1);
  //   }
  //   else{
  //     printf("%d:wrong! the answer is %u, gets %u\n",cnt++,atoi(arg1),ans1);
  //     exit(0);
  //   }
  //   memset(buf,0,sizeof(buf));
  // }
  /***************end test***************/
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
