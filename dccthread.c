#include <ucontext.h>
#include <stdlib.h>
#include "dccthread.h"
#include "dlist.h"

struct dccthread {
    ucontext_t *uc;
    char *name;
};

//struct dlist *threadList = NULL;
struct dlist *readyQueue = NULL;
dccthread_t *currentThread = NULL;
dccthread_t *managerThread = NULL;

void dccthread_init(void (*func)(int), int param) {
    //threadList = dlist_create();
    readyQueue = dlist_create();
    managerThread = dccthread_create("gerente", dccthread_scheduler, 0);
    dccthread_create("main", func, param);
    setcontext(managerThread->uc);
    //setcontext(dccthread_create("main", func, param)->uc);
}

dccthread_t *dccthread_create(const char *name, void (*func)(int), int param) {
    dccthread_t *newThread = malloc(sizeof (dccthread_t));
    newThread->uc = malloc(sizeof (ucontext_t));
    newThread->name = name;
    getcontext(newThread->uc);

    if (name == "gerente") {
        newThread->uc->uc_link = NULL;
    } else {
        newThread->uc->uc_link = managerThread->uc; // set to manager thread
    }

    newThread->uc->uc_stack.ss_sp = (char *) malloc(THREAD_STACK_SIZE); //(char *)
    newThread->uc->uc_stack.ss_size = THREAD_STACK_SIZE;
    makecontext(newThread->uc, func, 1, param);

    if (name != "gerente") {
        dlist_push_right(readyQueue, newThread);
    }

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

void dccthread_scheduler(int dummy) {
    while (1) {
        if (dlist_empty(readyQueue)) break;

        // if (currentThread == NULL) {
        //     currentThread = dlist_pop_left(readyQueue);
        //     setcontext(currentThread->uc);
        // } else {
        //     swapcontext(managerThread->uc, );
        // }

        currentThread = dlist_pop_left(readyQueue);
        swapcontext(managerThread->uc, currentThread->uc);
    }
}
