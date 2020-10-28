#include <am.h>
#include <nemu.h>
uint64_t boot_time;
void __am_timer_init() {
  boot_time=inl(RTC_ADDR);
  boot_time=boot_time==999000?1000000:boot_time; 
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t now=inl(RTC_ADDR);
  now=now>=999000?1000000:now;
  uptime->us=now-boot_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
