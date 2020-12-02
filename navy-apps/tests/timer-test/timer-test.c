#include <stdio.h>
#include <sys/time.h>


int main(){
    struct timeval tv;
    int cur=0;
    while(1){
        gettimeofday(&tv, NULL);
        if(tv.tv_sec==cur){
            cur++;
            printf("%ds %dus\n",tv.tv_sec,tv.tv_usec);
        }
    }
}

