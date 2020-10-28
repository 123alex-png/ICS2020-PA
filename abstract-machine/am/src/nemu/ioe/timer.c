#include <am.h>
#include <nemu.h>

#include<stdio.h>
void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  if(inl(RTC_ADDR)==999999){
    outl(RTC_ADDR,0);
  }
  uptime->us+=1000000;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
