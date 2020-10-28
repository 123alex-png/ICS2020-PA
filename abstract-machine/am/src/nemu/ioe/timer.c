#include <am.h>
#include <nemu.h>

#include<stdio.h>
int boot_time;
int second=0;
int last;
void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  int now=inl(RTC_ADDR);
  printf("%d\n",now);
  if(now==1052192){
    second++;
  }
  uptime->us=second*1000000+now;;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
