#include "dccthread.h"
#include "dlist.h"

struct dccthread {
    ucontext_t *uc;
    const char *name;
    dccthread_t *waiting;
};

timer_t timerID;
struct sigevent sev;


struct itimerspec its;
struct sigaction sa;

struct dlist *threadQueue = NULL;
dccthread_t *currentThread = NULL;
dccthread_t *managerThread = NULL;

void dccthread_init(void (*func)(int), int param) {
    threadQueue = dlist_create();
    managerThread = dccthread_create("gerente", dccthread_scheduler, 0);
    dccthread_create("main", func, param);
    dccthread_start_timer();
    setcontext(managerThread->uc);
}

dccthread_t *dccthread_create(const char *name, void (*func)(int), int param) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGEV_THREAD);
    sigprocmask(SIG_BLOCK, &set, NULL);
    
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
        dlist_push_right(threadQueue, newThread);
    }

    sigprocmask(SIG_UNBLOCK, &set, NULL);

    return newThread;
}

void dccthread_yield(void) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGEV_THREAD);
    sigprocmask(SIG_BLOCK, &set, NULL);

    dlist_push_right(threadQueue, dccthread_self());
    swapcontext(dccthread_self()->uc, managerThread->uc);

    sigprocmask(SIG_UNBLOCK, &set, NULL);
}

dccthread_t *dccthread_self(void) {
    return currentThread;
}

const char *dccthread_name(dccthread_t *tid) {
    return tid->name;
}

void dccthread_scheduler(int dummy) {
    int free_thread_found = 0;
    
    while (1) {
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGEV_THREAD);
        sigprocmask(SIG_BLOCK, &set, NULL);

        if (dlist_empty(threadQueue)) break;

        free_thread_found = 0;
        while (!free_thread_found) {
            dccthread_t *aux = dlist_pop_left(threadQueue);
            if (aux->waiting != NULL) {
                if (dlist_find(threadQueue, aux->waiting, dccthread_cmp)) {
                    dlist_push_right(threadQueue, aux);
                } else {
                    currentThread = aux;
                    free_thread_found = 1;
                    currentThread->waiting = NULL;
                }
            } else {
                currentThread = aux;
                free_thread_found = 1;
            }
        }

        sigprocmask(SIG_UNBLOCK, &set, NULL);
        swapcontext(managerThread->uc, currentThread->uc);
    }
}

void dccthread_exit(void) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGEV_THREAD);
    sigprocmask(SIG_BLOCK, &set, NULL);

    sigprocmask(SIG_UNBLOCK, &set, NULL);
    setcontext(managerThread->uc);
}

void dccthread_wait(dccthread_t *tid) {
    dccthread_self()->waiting = tid;
    dccthread_yield();
}

int dccthread_cmp(const dccthread_t *t1, const dccthread_t *t2) {
    return t1 == t2;
}

void dccthread_start_timer(void) {
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = dccthread_yield;
    //sev.sigev_notify = SIGEV_SIGNAL;
    //sev.sigev_signo = SIGRTMIN;

    //sa.sa_flags = SA_SIGINFO;
    //sa.sa_sigaction = dccthread_handler;
    //sigemptyset(&sa.sa_mask);

    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 1;
    its.it_interval.tv_sec  = 0;
    its.it_interval.tv_nsec = 1; // 1000000

    //sigaction(SIGRTMIN, &sa, NULL);
    timer_create(CLOCK_PROCESS_CPUTIME_ID, &sev, &timerID);
    timer_settime(timerID, 0, &its, NULL);
}
