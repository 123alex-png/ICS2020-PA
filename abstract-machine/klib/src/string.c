#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  const char *p;
  for(p=s;;p++){
    if(*p=='\0'){
      return p-s;
    }
  }
  return 0;
}

char *strcpy(char* dst,const char* src) {
  return NULL;
}

char* strncpy(char* dst, const char* src, size_t n) {
  return NULL;
}

char* strcat(char* dst, const char* src) {
  return NULL;
}

int strcmp(const char* s1, const char* s2) {
  if(strlen(s1)!=strlen(s2))return 1;
  const char *p1=s1, *p2=s2;
  while(*p1!='\0'){
    if(*p1!=*p2){
      return 1;
    }
    p1++;
    p2++;
  }
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  return 0;
}

void* memset(void* v,int c,size_t n) {
  return NULL;
}

void* memmove(void* dst,const void* src,size_t n) {
  return NULL;
}

void* memcpy(void* out, const void* in, size_t n) {
  char *dest=(char*)out;
  const char *src=(char*)in;
  do{
    *dest++=*src++;
  }while(--n>0);
  return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  const char *p1=(char *)s1,*p2=(char *)s2;
  do{
    if(*p1++!=*p2++){
      return 1;
    }
  }while(--n>0);
  return 0;
}

#endif
