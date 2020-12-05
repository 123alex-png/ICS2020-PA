#include <stdio.h>
#include <NDL.h>

int main(){
    //int cur=1000;
    int cnt=0;
    while(1){
        //gettimeofday(&tv, NULL);
        uint32_t time=NDL_GetTicks();
        printf("%dms\n", time);
        sleep(1);
        //assert(0);
    }
}

