//
// Created by os on 6/20/24.
//

#ifndef _SEM_H
#define _SEM_H

#include "list.hpp"
#include "ListOfSleepingSemaphoreThreads.h"
#include "tcb.hpp"

class _sem {
public:
    explicit _sem(unsigned init=1): value(init), closed(false), numOfBlocked(0),timeBlockedThreads(this) {}
    ~_sem(){close();}

    int wait();
    int signal();
    int close();
    int timedWait(time_t timeout);

    int getValue() const { return value;}

    static _sem* createSemaphore(unsigned init);

    int SEMDEAD = -1,TIMEOUT = -2;

private:
    void block();
    void time_block(time_t timeout);
    void deblock();
    int value;
    bool closed;
    int numOfBlocked;
    List<TCB> blockedThreads;
    ListOfSleepingSemaphoreThreads timeBlockedThreads;
    friend class Riscv;
    friend class ListOfSleepingSemaphoreThreads;
};



#endif //_SEM_H
