#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#define MAXSPN 2
// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int cnt;

uint32_t choose(uint32_t n){
  uint32_t ret=rand()%n;
  return ret;
}

static inline void gen_num(){
  for(int i=0;i<choose(MAXSPN);i++){
    buf[cnt++]=' ';
  }
  char tmp[40];
  sprintf(tmp,"%u",choose(0x3f3f3f3f));
  for(int i=0;i<strlen(tmp);i++){
    buf[cnt++]=tmp[i];
  }
  buf[cnt++]='u';
  for(int i=0;i<choose(MAXSPN);i++){
    buf[cnt++]=' ';
  }
}

static inline void gen_rand_op(){
  for(int i=0;i<choose(MAXSPN);i++){
    buf[cnt++]=' ';
  }
  switch(choose(4)){
    case 0: {buf[cnt++]='+';break;}
    case 1: {buf[cnt++]='-';break;}
    case 2: {buf[cnt++]='*';break;}
    default: {buf[cnt++]='/';break;}
  }
  for(int i=0;i<choose(MAXSPN);i++){
    buf[cnt++]=' ';
  }
}

static inline void gen_lstoken(){
  for(int i=0;i<choose(MAXSPN);i++){
    buf[cnt++]=' ';
  }
  buf[cnt++]='(';
  for(int i=0;i<choose(MAXSPN);i++){
    buf[cnt++]=' ';
  }
}

static inline void gen_rstoken(){
  for(int i=0;i<choose(MAXSPN);i++){
    buf[cnt++]=' ';
  }
  buf[cnt++]=')';
  for(int i=0;i<choose(MAXSPN);i++){
    buf[cnt++]=' ';
  }
}

static inline void gen_rand_expr() {
  //buf[0] = '\0';
  if(cnt>60000)return;
  switch(choose(3)){
    case 0: {
      gen_num();
      break;
    }
    case 1: {
      gen_lstoken();
      gen_rand_expr();
      gen_rstoken();
      break;
    }
    default: {
      gen_rand_expr();
      gen_rand_op();
      gen_rand_expr();
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    cnt=0;
    memset(buf,0,sizeof(buf));
    memset(code_buf,0,sizeof(code_buf));
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -Werror -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
