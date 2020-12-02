#include <stdio.h>
#include <NDL.h>

int main(){
    int cur=1000;
    while(1){
        //gettimeofday(&tv, NULL);
        uint32_t time=NDL_GetTicks();
        if(time==cur){
            cur+=1000;
            printf("%dms\n", time);
        }
    }
}

