//
// Created by os on 6/21/24.
//

#ifndef LISTOFSLEEPINGTHREADS_H
#define LISTOFSLEEPINGTHREADS_H

#include "syscall_c.h"

class ListOfSleepingThreads {

protected:
    struct Elem {
        TCB* thread;
        time_t timeLeft;
        Elem* next;
        Elem(TCB* thread, time_t timeLeft): thread(thread), timeLeft(timeLeft), next(nullptr) {}
    };

    Elem *head, *tail;
public:
    ListOfSleepingThreads(): head(nullptr), tail(nullptr) {}

    void put(TCB* thread, time_t timeSlice);

    void decFirst();

    time_t peekFirstTime();

    virtual void removeFinished();

    int remove(TCB* thread);
};


#endif //LISTOFSLEEPINGTHREADS_H
