#include <ucontext.h>
#include <stdlib.h>
#include "dccthread.h"
#include "dlist.h"

struct dccthread {
    ucontext_t *uc;
    char *name;
};

struct dlist *threadList;
dccthread_t *currentThread = NULL;

void dccthread_init(void (*func)(int), int param) {
    threadList = dlist_create();
    dccthread_create("gerente", NULL, 0);
    setcontext(dccthread_create("principal", func, param)->uc);
}

dccthread_t *dccthread_create(const char *name, void (*func)(int), int param) {
    dccthread_t *newThread = malloc(sizeof (dccthread_t));
    newThread->uc = malloc(sizeof (ucontext_t));
    newThread->name = name;
    getcontext(newThread->uc);

    newThread->uc->uc_link = NULL; // set to manager thread
    newThread->uc->uc_stack.ss_sp = (char *) malloc(THREAD_STACK_SIZE);
    newThread->uc->uc_stack.ss_size = THREAD_STACK_SIZE;
    makecontext(newThread->uc, func, 1, param);
    
    dlist_push_right(threadList, newThread);

    return newThread;
}

void dccthread_yield(void) {
}

dccthread_t *dccthread_self(void) {
    return currentThread;
}

const char *dccthread_name(dccthread_t *tid) {
    return tid->name;
}
