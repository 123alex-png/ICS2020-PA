#include <isa.h>
#include<string.h>
#include<stdlib.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_UNEQ,
  TK_NUM, TK_REG, TK_16, TK_AND, TK_STRING,
  TK_LE, TK_GE, TK_DEREF, TK_HEX
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},   // spaces
  {"\\+", '+'},        // plus
  {"==", TK_EQ},       // equafl
  {"<=", TK_LE},       //le
  {">=", TK_GE},       //ge
  {"-",'-'},           //minus
  {"\\*",'*'},         //mulitply or dereference
  {"/",'/'},           //divide
  {"0x",TK_16},        //hexadecimal number

  {"[0-9]+",TK_NUM},   //number
  {"[a-zA-Z]+",TK_STRING},//which reg
  {"\\(",'('},         //left brackets
  {"\\)",')'},         //right brackets
  {"\\u",'u'},        
  {"\\$",TK_REG},      //regs
  {"!=",TK_UNEQ},      // unequafl
  {"&&", TK_AND},      //and  
  {"<",'<'},           //<
  {">",'>'}            //>    
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */

word_t string16tonum(char *a){
  word_t ret=0;
  for(int i=0;i<strlen(a);i++){
    word_t tmp;
    switch (a[i])
    {
    case 'a':case 'A':{
      tmp=10;
      break;
    }
    case 'b':case 'B':{
      tmp=11;
      break;
    }
    case 'c':case 'C':{
      tmp=12;
      break;
    }
    case 'd':case 'D':{
      tmp=13;
      break;
    }
    case 'e':case 'E':{
      tmp=14;
      break;
    }
    case 'f':case 'F':{
      tmp=15;
    }
    default:
      tmp=(word_t)(a[i]-'0');
      break;
    }
    ret=16*ret+tmp;
  }
  return ret;
}

void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  memset(tokens,0,sizeof(tokens));
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
         switch (rules[i].token_type) {
          case 256:break;
          case 'u':break;
          case TK_NUM:{
            if(tokens[nr_token-1].type==TK_HEX){//already a hexadecimal number , just connect to it
              strncat(tokens[nr_token-1].str,substr_start,substr_len);
            }
            else if(tokens[nr_token-1].type==TK_16){//before is a "0x"
              memset(tokens[nr_token-1].str,0,sizeof(tokens[nr_token].str));
              strncpy(tokens[nr_token-1].str,substr_start,substr_len);
              tokens[nr_token-1].type=TK_HEX;
              break;
            }
            else{//just a number
              strncpy(tokens[nr_token].str,substr_start,substr_len);
              tokens[nr_token++].type=rules[i].token_type;
            }
            break;
          }
          case TK_STRING:{
            if(tokens[nr_token-1].type==TK_HEX){//already a hexadecimal number , just connect to it
              strncat(tokens[nr_token-1].str,substr_start,substr_len);
            }
            else if(tokens[nr_token-1].type==TK_16){//before is a "0x"
              memset(tokens[nr_token-1].str,0,sizeof(tokens[nr_token].str));
              strncpy(tokens[nr_token-1].str,substr_start,substr_len);
              tokens[nr_token-1].type=TK_HEX;
              break;
            }
            else{//before is a "$",replace it
              strncpy(tokens[nr_token-1].str,substr_start,substr_len);
              tokens[nr_token-1].type=TK_REG;
            }
            break;
          }
          default: strncpy(tokens[nr_token].str,substr_start,substr_len);
                   tokens[nr_token++].type=rules[i].token_type;
                   break;
         }
        //  if(!flag&&tokens[nr_token-2].type==TK_16){
        //    char buf[50];
        //    word_t num=string16tonum(tokens[nr_token-1].str);
        //    sprintf(buf,"%u",num);
        //    strcpy(tokens[nr_token-2].str,buf);
        //    tokens[nr_token-2].type=TK_NUM;
        //    nr_token--;
        //  }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool judge(int p,int q){
  int head=0;
  for(int i=p;i<=q;i++){
    if(tokens[i].type=='('){
      head++;
    }
    if(tokens[i].type==')'){
      if(head==0){
        return 0;
      }
      else{
        head--;
      }
    }
  }
  return !head;
}
static word_t eval(int p,int q,bool *success){
  if(*success==false){
    return 1;
  }
  if(p>q){
    *success=false;
    return 1;
  }
  else if(p==q){
    if(tokens[p].type!=TK_NUM){
      if(tokens[p].type!=TK_REG){
        if(tokens[p].type!=TK_HEX){
        *success=false;
        return 1;
        }
        else{
           word_t num=string16tonum(tokens[nr_token-1].str);
           return num;
        }
      }
      else{
        return isa_reg_str2val(tokens[p].str,success);
      }
    }
    return (word_t)atoi(tokens[p].str);
  }
  else if(tokens[p].type=='('&&tokens[q].type==')'&&judge(p+1,q-1)){
    return eval(p+1,q-1,success);
  }
  else{
    int i;
    int flag1=0,head=0,op=-1;
    for(i=q;i>=p;i--){
      if(tokens[i].type=='('){
        head++;
      }
      if(tokens[i].type==')'){
        head--;
      }
      if(!head&&tokens[i].type==TK_DEREF){
        op=i;
        flag1=1;
        break;
      }
    }
    if(!flag1){
      for(i=q;i>=p;i--){
        if(tokens[i].type=='('){
          head++;
        }
        if(tokens[i].type==')'){
          head--;
        }
          if(!head&&(tokens[i].type==TK_EQ||tokens[i].type==TK_UNEQ||tokens[i].type==TK_AND||tokens[i].type==TK_LE||tokens[i].type==TK_GE||tokens[i].type=='<'||tokens[i].type=='>')){
          op=i;
          flag1=1;
          break;
        }
      }
    }
    if(!flag1){
      for(i=q;i>=p;i--){
        if(tokens[i].type=='('){
          head++;
        }
        if(tokens[i].type==')'){
          head--;
        }
        if(!head&&(tokens[i].type=='+'||tokens[i].type=='-')){
          op=i;
          flag1=1;
          break;
        }
      }
    }
    if(!flag1){
      for(i=q;i>=p;i--){
      if(tokens[i].type=='('){
        head++;
      }
      if(tokens[i].type==')'){
        head--;
      }
      if(!head&&(tokens[i].type=='*'||tokens[i].type=='/')){
        op=i;
        break;
      }
    }
    }
    if(op==-1){
      *success=false;
      return 1;
    }
    else{
      word_t val1=eval(p,op-1,success);
      word_t val2=eval(op+1,q,success);
      switch (tokens[op].type)
      {
      case TK_DEREF:{
        return paddr_read(eval(op+1,q,success),4);
      }
      case '+':{
        return val1+val2;
        break;
      }
      case '-':{
        return val1-val2;
        break;
      }
      case '*':{
        return val1*val2;
        break;
      }
      case '/':{
        if(val2==0){
          printf("Division by zero\n");
          *success=false;
          return 0;
        }
        return val1/val2;
        break;
      }
      case TK_EQ:{
        return val1==val2;
        break;
      }
      case TK_UNEQ:{
        return val1!=val2;
        break;
      }
      case TK_AND:{
        return val1&&val2;
        break;
      }
      case TK_LE:{
        return val1<=val2;
        break;
      }
      case TK_GE:{
        return val1>=val2;
        break;
      }
      case '<':{
        return val1<val2;
        break;
      }
      case '>':{
        return val1>val2;
        break;
      }
      default:{
        return 1;
        break;
      }
      }
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0 || (tokens[i - 1].type != TK_NUM && tokens[i -1].type != TK_STRING) ) ) {
      tokens[i].type = TK_DEREF;
    }
  }
  if(!judge(0,nr_token-1)){
    *success=false;
    return 0;
  }
  word_t ans=eval(0,nr_token-1,success);

  return ans;
}
