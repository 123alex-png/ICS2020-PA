#include "watchpoint.h"
#include "expr.h"
#include <stdlib.h>

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


WP *new_pool(){
  assert(free_);
  head=free_;
  free_=free_->next;
  free_->prev=head;
  return head;
}

void free_wp(WP *wp){
  if(wp->prev){
    wp->prev->next=wp->next;
  }
  if(wp->next){
    wp->next->prev=wp->prev;
  }
  head->next=wp;
  wp->next=free_;
  free_=wp;
}

bool check_wp(){
  bool success=true;
  WP *tmp=head;
  bool flag=false;
  while(tmp){
    word_t curval=expr(tmp->expr,&success);
    if(curval!=tmp->val){
      tmp->hit++;
      printf("Old value = %u\n",tmp->val);
      printf("New value = %u\n",curval);
      flag=true;
    }
    tmp=tmp->prev;
  }
  return flag;
}

void wp_display(){
  WP *tmp=head;
  if(tmp){
    printf("Num                 What\n");
  }
  else{
    printf("No watchpoints\n");
  }
  while(tmp){
    printf("%d                 %s                 breakpoint already hit %d times\n",tmp->NO,tmp->expr,tmp->hit);
    tmp=tmp->prev;
  }
}

void delete_wp(char *args){
  int num=atoi(args);
  int flag=0;
  for(WP *tmp=head;tmp;tmp=tmp->prev){
    if(tmp->NO==num){
      free_wp(tmp);
      flag=1;
      break;
    }
  }
  if(!flag){
    printf("NO watchpoint number %d\n",num);
  }
}
