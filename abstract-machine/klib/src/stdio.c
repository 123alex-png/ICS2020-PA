#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  // va_list arg;
  int cnt=0;
  // va_start(arg,fmt);
  // for(size_t i=0;fmt[i]!='\0';i++){
    // if(fmt[i]!='%'){
    //   out[cnt++]=fmt[i];
    // }
    // else{
      // int i=1;
      // switch(fmt[i]){
      //   case 'd':{
          int tmp=/*va_arg(arg,int)*/10;
          if(tmp<0){
            out[cnt++]='-';
            tmp=(unsigned)(-tmp);
          }
          else{
            tmp=(unsigned)tmp;
          }
          char digit[15];//from low to high
          int pos=0;
          do{
            digit[pos++]=tmp%10+'0';
            tmp/=10;

            // digit[0]=tmp%10+'0';
            // tmp/=10;
            // digit[1]=tmp%10+'0';
            // tmp/=10;
            
          }while(tmp!=0);
          // out[cnt++]=digit[1];
          // out[cnt++]=digit[0];
          // break;
          while(pos>0){
            --pos;
            out[cnt++]=digit[pos];
          }//break;
        // }
        // case 's':{
        //   char *tmp=va_arg(arg,char *);
        //   for(size_t i=0;*(tmp+i)!='\0';i++){
        //     out[cnt++]=*(tmp+i);
        //   }
        //   break;
        // }
      // }
  //   }
  // }
  out[cnt++]='\0';
  // va_end(arg);
  return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  return 0;
}

#endif
