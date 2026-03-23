//
// Created by os on 6/20/24.
//

#include "../h/_sem.h"
#include "../h/syscall_c.h"

_sem * _sem::createSemaphore(unsigned init) {
    return new _sem(init);
}

void _sem::block() {
    numOfBlocked++;
    TCB::running->setBlocked(true);
    blockedThreads.addLast(TCB::running);
    thread_dispatch();
}

void _sem::time_block(time_t timeout) {
    numOfBlocked++;
    TCB::running->setSleeping(true);
    TCB::running->setBlocked(true);
    blockedThreads.addLast(TCB::running);
    timeBlockedThreads.put(TCB::running, timeout);
    thread_dispatch();
}

void _sem::deblock() {
    numOfBlocked--;
    TCB *temp = blockedThreads.removeFirst();
    int retValue = timeBlockedThreads.remove(temp);
    temp->setBlocked(false);
    temp->setSleeping(false);
    if(retValue!=-1)temp->setSemValue(retValue);
    Scheduler::put(temp);
}

int _sem::wait() {
    if(closed) return -1;
    value--;
    if(value<0) {
        block();
    }
    if(!closed) return 0;
    if(numOfBlocked!=0) {
        numOfBlocked--;
        return -1;
    }
    return 0;
}

int _sem::signal() {
    if(closed) return -1;
    value++;
    if(value<=0) {
        deblock();
    }
    return 0;
}

int _sem::close() {
    if(closed) return -1;
    closed = true;
    if(blockedThreads.peekFirst()) {
        timeBlockedThreads.removeAll();
        while(blockedThreads.peekFirst()) {
            blockedThreads.peekFirst()->setBlocked(false);
            blockedThreads.peekFirst()->setSleeping(false);
            Scheduler::put(blockedThreads.peekFirst());
            blockedThreads.removeFirst();
        }
    }
    return 0;
}

int _sem::timedWait(time_t timeout) {
    if(closed) return -1;
    int retValue;
    value--;
    if(value<0) {
        time_block(timeout);
    }
    retValue = TCB::running->getSemValue();
    if(!closed) return retValue;
    if(numOfBlocked!=0) {
        numOfBlocked--;
        return SEMDEAD;
    }
    return retValue;
}


