#include <ucontext.h>
#include "dccthread.h"
#include "dlist.h"

struct dccthread {
    ucontext_t uc;
    char *name;
    void (*function)(int);
};

struct dlist *threadList;

void dccthread_init(void (*func)(int), int param) {
    threadList = dlist_create();
    
}

dccthread_t *dccthread_create(const char *name, void (*func)(int), int param) {
    dccthread_t *newThread;
    newThread->name = name;
    newThread->function = func;
    //newThread->uc = ??;
    

    return newThread;
}


void dccthread_yield(void) {

}

dccthread_t *dccthread_self(void) {

}

const char *dccthread_name(dccthread_t *tid) {
    return tid->name;
}
