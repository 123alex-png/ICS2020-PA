#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
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
  return -1;
}

static int cmd_help(char *args);

static int cmd_execute_once(char* args);

static int cmd_info(char* args);

static int cmd_x(char* args);
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
      printf("0x%p: ",&(cpu.eax)+offset);
      for(k=0;k<4;k++){
        printf("0x%d      ",*(&(cpu.eax)+offset));
        offset+=4;
      }
      printf("\n");
    }
    printf("0x%p): ",&(cpu.eax)+offset);
    for(k=0;k<left;k++){
      printf("0x%d      ",*(&(cpu.eax)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$ecx")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("0x%p: ",&(cpu.ecx)+offset);
      for(k=0;k<4;k++){
        printf("0x%d      ",*(&(cpu.ecx)+offset));
        offset+=4;
      }
      printf("\n");
    }
    printf("0x%p: ",&(cpu.ecx)+offset);
    for(k=0;k<left;k++){
      printf("0x%d      ",*(&(cpu.ecx)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$edx")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("0x%p: ",&(cpu.edx)+offset);
      for(k=0;k<4;k++){
        printf("0x%d      ",*(&(cpu.edx)+offset));
        offset+=4;
      }
      printf("\n");
    }
    printf("0x%p: ",&(cpu.edx)+offset);
    for(k=0;k<left;k++){
      printf("0x%d      ",*(&(cpu.edx)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$ebx")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("0x%p: ",&(cpu.ebx)+offset);
      for(k=0;k<4;k++){
        printf("0x%d      ",*(&(cpu.ebx)+offset));
        offset+=4;
      }
      printf("\n");
    }
    printf("0x%p: ",&(cpu.ebx)+offset);
    for(k=0;k<left;k++){
      printf("0x%d      ",*(&(cpu.ebx)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$esp")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("0x%p: ",&(cpu.esp)+offset);
      for(k=0;k<4;k++){
        printf("0x%d      ",*(&(cpu.esp)+offset));
        offset+=4;
      }
      printf("\n");
    }
    printf("0x%p: ",&(cpu.esp)+offset);
    for(k=0;k<left;k++){
      printf("0x%d      ",*(&(cpu.esp)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$ebp")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("0x%p: ",&(cpu.ebp)+offset);
      for(k=0;k<4;k++){
        printf("0x%d      ",*(&(cpu.ebp)+offset));
        offset+=4;
      }
      printf("\n");
    }
    printf("0x%p: ",&(cpu.ebp)+offset);
    for(k=0;k<left;k++){
      printf("0x%d      ",*(&(cpu.ebp)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$esi")){
    int k;
    int cnt=n/4,left=n%4;
    int offset=0;
    for(j=0;j<cnt;j++){
      printf("0x%p: ",&(cpu.esi)+offset);
      for(k=0;k<4;k++){
        printf("0x%d      ",*(&(cpu.esi)+offset));
        offset+=4;
      }
      printf("\n");
    }
    printf("0x%p: ",&(cpu.esi)+offset);
    for(k=0;k<left;k++){
      printf("0x%d      ",*(&(cpu.esi)+offset));
      offset+=4;
    }
      printf("\n");
  }
  else if(!strcmp(arg2,"$edi")){
    int k;
    int offset=0;
    int cnt=n/4,left=n%4;
    for(j=0;j<cnt;j++){
      printf("0x%p: ",&(cpu.edi)+offset);
      for(k=0;k<4;k++){
        printf("0x%d      ",*(&(cpu.edi)+offset));
        offset+=4;
      }
      printf("\n");
    }
    printf("0x%p: ",&(cpu.edi)+offset);
    for(k=0;k<left;k++){
      printf("0x%d      ",*(&(cpu.edi)+offset));
      offset+=4;
    }
      printf("\n");
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
