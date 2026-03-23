//
// Created by os on 6/23/24.
//
#include "../h/ListOfSleepingSemaphoreThreads.h"
#include "../h/scheduler.hpp"
#include "../h/_sem.h"

void ListOfSleepingSemaphoreThreads::removeFinished() {
    while(head && head->timeLeft == 0) {
        Elem *temp = head;
        head = head->next;
        if(!head) tail = nullptr;
        temp->thread->setSleeping(false);
        temp->thread->setBlocked(false);
        Semafor->numOfBlocked--;
        Semafor->value++;
        Semafor->blockedThreads.remove(temp->thread);
        temp->thread->setSemValue(Semafor->TIMEOUT);
        Scheduler::put(temp->thread);
        delete temp;
    }
}

void ListOfSleepingSemaphoreThreads::removeAll() {
    while(head) {
        Elem *temp = head;
        head = head->next;
        if(!head) tail = nullptr;
        delete temp;
    }
}
