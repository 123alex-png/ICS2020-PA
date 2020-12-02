#include <stdio.h>
#include <NDL.h>
unsigned int sleep(int second);
int main(){
    //int cur=1000;
    while(1){
        //gettimeofday(&tv, NULL);
        uint32_t time=NDL_GetTicks();
        printf("%dms\n", time);
        if(time > 50)break;
        //sleep(1);
    }
}

