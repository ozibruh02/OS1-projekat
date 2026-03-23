//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"

TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;


TCB *TCB::createThread(Body body, void* arg, uint64* stack_space)
{
    return new TCB(body, arg, stack_space, TIME_SLICE);
}

void TCB::yield()
{
    thread_dispatch();
}

void TCB::outputFunction(void *) {
    while(true){
        while((*((char*)(CONSOLE_STATUS)) & CONSOLE_TX_STATUS_BIT)){
            char c = Riscv::putBuffer->getc();
            *((char*)CONSOLE_TX_DATA) = c;
        }
    }
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished() && !old->isBlocked() && !old->isSleeping()) { Scheduler::put(old); }
    running = Scheduler::get();
    if (old != running) {
        TCB::contextSwitch(&old->context, &running->context);
    }
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}
