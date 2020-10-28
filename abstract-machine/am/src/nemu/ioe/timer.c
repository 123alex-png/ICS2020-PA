#include <am.h>
#include <nemu.h>

#include<stdio.h>
uint32_t second=0;
uint32_t last=0;
//int boot_time;
void __am_timer_init() {
  //boot_time=inl(RTC_ADDR);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t now=inl(RTC_ADDR);
  printf("%d\n",now);
  if(now<last){
    second++;
  }
  last=now;
  uptime->us=1000000u*second+now;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
