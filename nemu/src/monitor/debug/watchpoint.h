#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  char *expr;
  word_t val;
  word_t hit;
  struct watchpoint *prev;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

} WP;


#endif
