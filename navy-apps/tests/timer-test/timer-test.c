#include <stdio.h>
#include <sys/time.h>


int main(){
    struct timeval *tv;
    while(1){
        gettimeofday(tv, NULL);
        printf("%ds %dus\n",tv->tv_sec,tv->tv_usec);
        sleep(1);
    }
}

