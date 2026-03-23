//
// Created by os on 6/21/24.
//
#include "../h/ListOfSleepingThreads.h"
#include "../h/tcb.hpp"

void ListOfSleepingThreads::put(TCB *thread, time_t timeSlice) {

    if(!head) {
        head = new Elem(thread, timeSlice);
        tail = head;
    }
    else {
        Elem *temp = head;
        Elem* prev = nullptr;
        while(temp != tail && timeSlice >= temp->timeLeft) {
            timeSlice -= temp->timeLeft;
            prev = temp;
            temp = temp->next;
        }
        if(temp == tail) {
            if(timeSlice >= temp->timeLeft) {
                Elem* elem = new Elem(thread,timeSlice-temp->timeLeft);
                tail->next = elem;
                tail = elem;
            }
            else {
                Elem* elem = new Elem(thread, timeSlice);
                elem->next = tail;
                if(prev) {
                    prev->next = elem;
                }
                else head = elem;
                tail->timeLeft -= timeSlice;
            }
        }
        else {
            Elem* elem = new Elem(thread, timeSlice);
            elem->next = temp;
            if(prev) prev->next = elem;
            else head = elem;
            temp->timeLeft -= timeSlice;
        }
    }
}

void ListOfSleepingThreads::decFirst() {
    if(head) head->timeLeft--;
}

time_t ListOfSleepingThreads::peekFirstTime() {
    if(!head) return -1;
    return head->timeLeft;
}

void ListOfSleepingThreads::removeFinished() {

    while(head && head->timeLeft == 0) {
        Elem *temp = head;
        head = head->next;
        if(!head) tail = nullptr;
        temp->thread->setSleeping(false);
        Scheduler::put(temp->thread);
        delete temp;
    }
}

int ListOfSleepingThreads::remove(TCB *thread) {
    int retValue = 0;
    Elem *temp = head;
    Elem* prev = nullptr;
    while(temp != tail && thread != temp->thread) {
        prev = temp;
        temp = temp->next;
    }
    if(!temp) retValue = -1;
    else if(temp->timeLeft == 0) retValue = -2;
    if(temp == head) {
        if(head==tail) head = tail = nullptr;
        else {
            head->next->timeLeft += head->timeLeft;
            head = head->next;
        }
        delete temp;
    }
    else if(temp == tail) {
        prev->next = nullptr;
        tail = prev;
        delete temp;
    }
    else {
        temp->next->timeLeft += temp->timeLeft;
        prev->next = temp->next;
        delete temp;
    }
    return retValue;
}
