#include <am.h>
#include <nemu.h>

//#include<stdio.h>
// uint32_t boot_time_s;
// uint32_t boot_time_us;
uint32_t now_s;
uint32_t now_us;
void __am_timer_init() {
  // boot_time_us=inl(RTC_ADDR);
  // boot_time_s=inl(RTC_ADDR+4);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  now_us=inl(RTC_ADDR);
  now_s=inl(RTC_ADDR+4);
  uptime->us = (now_s/*-boot_time_s*/) * 1000000 + (now_us/*-boot_time_us*/ + 500);

  //printf("%d\n",now);
  // if(now<last){
  //   second++;
  // }
  // last=now;
  // uptime->us=1000000u*second+now;

}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
