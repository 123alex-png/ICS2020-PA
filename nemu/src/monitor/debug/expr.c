#include <isa.h>
#include<string.h>
#include<stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_UNEQ,
  TK_NUM, TK_REG, TK_16, TK_AND, TK_STRING
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
  {"-",'-'},           //minus
  {"\\*",'*'},         //mulitply or dereference
  {"/",'/'},           //divide
  {"0x",TK_16},        //hexadecimal number
  {"[0-9]+",TK_NUM},   //number
  {"[a-z]",TK_STRING},  //which reg
  {"\\(",'('},         //left brackets
  {"\\)",')'},         //right brackets
  {"\\u",'u'},        
  {"\\$",TK_REG},      //regs
  {"!=",TK_UNEQ},      // unequafl
  {"&&", TK_AND},      //and       
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
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

static Token tokens[32000] __attribute__((used)) = {};
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
            if(tokens[nr_token-1].type==TK_16){
              int num_10=0;
              for(char *i=substr_start;i<substr_len+substr_start;i++){
                num_10=16*num_10+(*i-'0');
              }
              sprintf(tokens[nr_token-1].str,"%d",num_10);
              tokens[nr_token-1].type=TK_NUM;
            }
            else{
              strncpy(tokens[nr_token].str,substr_start,substr_len);
              tokens[nr_token++].type=rules[i].token_type;
            }
          }
          default: strncpy(tokens[nr_token].str,substr_start,substr_len);
                   tokens[nr_token++].type=rules[i].token_type;
         }
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
      *success=false;
      return 1;
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
        if(!head&&(tokens[i].type==TK_EQ||tokens[i].type==TK_UNEQ||tokens[i].type==TK_AND)){
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
  if(!judge(0,nr_token-1)){
    *success=false;
    return 0;
  }
  word_t ans=eval(0,nr_token-1,success);

  return ans;
}
