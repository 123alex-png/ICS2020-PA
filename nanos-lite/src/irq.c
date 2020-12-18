#include <common.h>
void do_syscall(Context *c);
static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_SYSCALL: do_syscall(c);break;
    case EVENT_YIELD: printf("EVENT YIELD\n");break;
    //case EVENT_EXIT:printf("EVENT_EXIT\n");break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
