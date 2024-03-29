#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include "monitor/monitor.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

void cpu_exec(uint64_t);
void wp_display();
void delete_wp(char *args);
WP *new_pool();
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state=NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static int cmd_execute_once(char* args);

static int cmd_info(char* args);

static int cmd_x(char* args);

static int cmd_p(char* args);

static int cmd_w(char *args);

static int cmd_d(char *args){
  delete_wp(args);
  return 0;
}

static int cmd_b(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute one instruction", cmd_execute_once },
  { "info", "Print the information of the registers or the watchpoints", cmd_info},
  { "x", "Examine memory: x/FMT ADDRESS",cmd_x},
  { "p", "print the value of the expression", cmd_p},
  { "w", "add a watchpoint", cmd_w},
  { "d", "delete a watchpoint", cmd_d},
  { "b", "set a breakpoint", cmd_b}
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_execute_once(char* args){
  char *arg=strtok(NULL," ");
  int i;
  if( arg == NULL || !strcmp(arg,"1") ){
    isa_exec_once();
  }
  else{
    int j;
    int n=0,base=1;
    for(j=strlen(arg)-1;j>=0;j--){
      n+=base*(arg[j]-'0');
      base*=10;
    }
    for(i=0;i<n;i++){
      isa_exec_once();
    }
  }
  return 0;
}

static int cmd_info(char* args){
  char *arg=strtok(NULL," ");
  if(arg == NULL){
    printf("\"info\" must be followed by the name of an info command, please type -r or -w\n");
  }
  else if(!strcmp(arg,"r")){
    isa_reg_display();
  }
  else if(!strcmp(arg,"w")){
    wp_display();
  }
  return 0;
}

static int cmd_x(char* args){
  char *arg1=strtok(NULL," ");
  char *arg2=strtok(NULL," ");
  if(arg1==NULL){
    printf("Argument required\n");
    return 0;
  }
  if(arg2==NULL){
    arg2=arg1;
    arg1="1";
  }
    int j;
    int n=0,base=1;
    for(j=strlen(arg1)-1;j>=0;j--){
      n+=base*(arg1[j]-'0');
      base*=10;
    }
  if(!strcmp(arg2,"$eax")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("%p: ",&(cpu.eax)+offset);
      for(k=0;k<4;k++){
        printf("0x%-15x      ",*(&(cpu.eax)+offset));
        offset+=4;
      }
      printf("\n");
    }
    if(left)
    printf("%p: ",&(cpu.eax)+offset);
    for(k=0;k<left;k++){
      printf("0x%-15x      ",*(&(cpu.eax)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$ecx")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("%p: ",&(cpu.ecx)+offset);
      for(k=0;k<4;k++){
        printf("0x%-15x      ",*(&(cpu.ecx)+offset));
        offset+=4;
      }
      printf("\n");
    }
    if(left)
    printf("%p: ",&(cpu.ecx)+offset);
    for(k=0;k<left;k++){
      printf("0x%-15x      ",*(&(cpu.ecx)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$edx")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("%p: ",&(cpu.edx)+offset);
      for(k=0;k<4;k++){
        printf("0x%-15x      ",*(&(cpu.edx)+offset));
        offset+=4;
      }
      printf("\n");
    }
    if(left)
    printf("%p: ",&(cpu.edx)+offset);
    for(k=0;k<left;k++){
      printf("0x%-15x      ",*(&(cpu.edx)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$ebx")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("%p: ",&(cpu.ebx)+offset);
      for(k=0;k<4;k++){
        printf("0x%-15x      ",*(&(cpu.ebx)+offset));
        offset+=4;
      }
      printf("\n");
    }
    if(left)
    printf("%p: ",&(cpu.ebx)+offset);
    for(k=0;k<left;k++){
      printf("0x%-15x      ",*(&(cpu.ebx)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$esp")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("%p: ",&(cpu.esp)+offset);
      for(k=0;k<4;k++){
        printf("0x%-15x      ",*(&(cpu.esp)+offset));
        offset+=4;
      }
      printf("\n");
    }
    if(left)
    printf("%p: ",&(cpu.esp)+offset);
    for(k=0;k<left;k++){
      printf("0x%-15x      ",*(&(cpu.esp)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$ebp")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("%p: ",&(cpu.ebp)+offset);
      for(k=0;k<4;k++){
        printf("0x%-15x      ",*(&(cpu.ebp)+offset));
        offset+=4;
      }
      printf("\n");
    }
    if(left)
    printf("%p: ",&(cpu.ebp)+offset);
    for(k=0;k<left;k++){
      printf("0x%-15x      ",*(&(cpu.ebp)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$esi")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("%p: ",&(cpu.esi)+offset);
      for(k=0;k<4;k++){
        printf("0x%-15x      ",*(&(cpu.esi)+offset));
        offset+=4;
      }
      printf("\n");
    }
    if(left)
    printf("%p: ",&(cpu.esi)+offset);
    for(k=0;k<left;k++){
      printf("0x%-15x      ",*(&(cpu.esi)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$edi")){
    int k;
    int offset=0;
    int cnt=n/4,left=n%4;
    for(j=0;j<cnt;j++){
      printf("%p: ",&(cpu.edi)+offset);
      for(k=0;k<4;k++){
        printf("0x%-15x      ",*(&(cpu.edi)+offset));
        offset+=4;
      }
      printf("\n");
    }
    if(left)
    printf("%p: ",&(cpu.edi)+offset);
    for(k=0;k<left;k++){
      printf("0x%-15x      ",*(&(cpu.edi)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else{
    if(arg2[0]=='0'&&arg2[1]=='x'){
      int input_addr=0,base16=1;
    for(j=strlen(arg2)-1;j>=2;j--){
      if(arg2[j]<='9'&&arg2[j]>='0')
        input_addr+=base16*(arg2[j]-'0');
      else
        input_addr+=base16*(arg2[j]-'a'+10);
      base16*=16;
    }
    //如何读取内存中的数据？？
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("0x%x: ",input_addr+offset);
      for(k=0;k<4;k++){
        printf("0x%-15x      ",paddr_read(input_addr+offset,4));
        offset+=4;
      }
      printf("\n");
    }
    if(left)
    printf("0x%x: ",input_addr+offset);
    for(k=0;k<left;k++){
        printf("0x%-15x      ",paddr_read(input_addr+offset,4));
        offset+=4;
    }
      printf("\n");
    }
  
  }
  return 0;
}

static int cmd_p(char* args){
  if(args==NULL){
    printf("Please type the expressions");
  }
  else{
    bool *success=(bool*)malloc(sizeof(bool));
    *success=true;
    word_t ans=expr(args,success);
      if(*success==false){
        printf("Invalid expression\n");
      }
      else{
        printf("%u\n",ans);
        if(ans>16)
        printf("0x%x\n",ans);
      }
      free(success);  
  }
  return 0;
}

static int cmd_w(char *args){
  bool success=true;
  word_t value=expr(args,&success);
  if(!success){
    printf("Invalid expreesion\n");
    return 0;
  }
  WP *wp=new_pool();
  strcpy(wp->expr,args);
  wp->val=value;
  printf("Watchpoint number %d : %s\n",wp->NO,args);
  return 0;
}

static int cmd_b(char *args){
  bool success=true;
  expr(args,&success);
  if(!success){
    printf("Invalid expreesion\n");
  }
  else{
    char arg[50]="cpu.pc==";
    strcat(arg,args);
    cmd_w(arg);
  }
  return 0;
}
void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);
    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif
    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
