#include <ucontext.h>
#include <stdlib.h>
#include "dccthread.h"
#include "dlist.h"

struct dccthread {
    ucontext_t *uc;
    const char *name;
    dccthread_t *waiting;
};

struct dlist *readyQueue = NULL;
dccthread_t *currentThread = NULL;
dccthread_t *managerThread = NULL;

void dccthread_init(void (*func)(int), int param) {
    readyQueue = dlist_create();
    managerThread = dccthread_create("gerente", dccthread_scheduler, 0);
    dccthread_create("main", func, param);
    setcontext(managerThread->uc);
}

dccthread_t *dccthread_create(const char *name, void (*func)(int), int param) {
    dccthread_t *newThread = malloc(sizeof (dccthread_t));
    newThread->uc = malloc(sizeof (ucontext_t));
    newThread->name = name;
    newThread->waiting = NULL;
    getcontext(newThread->uc);

    if (name == "gerente") {
        newThread->uc->uc_link = NULL;
    } else {
        newThread->uc->uc_link = managerThread->uc; // set to manager thread
    }

    newThread->uc->uc_stack.ss_sp = malloc(THREAD_STACK_SIZE);
    newThread->uc->uc_stack.ss_size = THREAD_STACK_SIZE;
    makecontext(newThread->uc, func, 1, param);

    if (name != "gerente") {
        dlist_push_right(readyQueue, newThread);
    }

    return newThread;
}

void dccthread_yield(void) {
    dlist_push_right(readyQueue, dccthread_self());
    swapcontext(dccthread_self()->uc, managerThread->uc);
}

dccthread_t *dccthread_self(void) {
    return currentThread;
}

const char *dccthread_name(dccthread_t *tid) {
    return tid->name;
}

void dccthread_scheduler(int dummy) {
    while (1) {
        if (dlist_empty(readyQueue)) break;

        currentThread = dlist_pop_left(readyQueue);
        swapcontext(managerThread->uc, currentThread->uc);
    }
}

void dccthread_exit(void)
{
    setcontext(managerThread->uc);
}

void dccthread_wait(dccthread_t *tid) {
    dccthread_self()->waiting = tid;
    dccthread_yield();
}