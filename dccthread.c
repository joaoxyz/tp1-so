#include <ucontext.h>
#include "dccthread.h"
#include "dlist.h"
#include <stdlib.h>

struct dccthread
{
  ucontext_t uc;
  char *name;
  void (*function)(int);
  int param;
};

struct dlist *threadList;

void dccthread_init(void (*func)(int), int param)
{
  threadList = dlist_create();
  gerente 
  principal

}

dccthread_t *dccthread_create(const char *name, void (*func)(int), int param)
{
  dccthread_t *newThread;
  newThread->name = name;
  newThread->function = func;
  newThread->param = param;
  // newThread->uc = ??;
  getcontext(newThread->uc);
  // stack_t *stack_pointer = malloc(sizeof(SIGSTKSZ));
  newThread->uc->uc_stack = malloc(sizeof(SIGSTKSZ));
  dlist_push_right(threadList, newThread);

  return newThread;
}

void dccthread_yield(void)
{
}

dccthread_t *dccthread_self(void)
{
}

const char *dccthread_name(dccthread_t *tid)
{
  return tid->name;
}
