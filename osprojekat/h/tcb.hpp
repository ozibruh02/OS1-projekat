//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"
#include "syscall_cpp.hpp"

// Thread Control Block
class TCB
{
public:
    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    bool isBlocked() const {return blocked;}

    void setBlocked(bool value) { blocked = value; }

    bool isSleeping() const {return sleeping;}

    void setSleeping(bool value) { sleeping = value; }

    int getSemValue() const {return semSleepValue;}

    void setSemValue(int value) { semSleepValue = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    void start() { Scheduler::put(this); }

    using Body = void (*)(void*);

    static TCB *createThread(Body body, void* arg, uint64* stack_space);

    static void yield();

    static TCB *running;

    static void outputFunction(void*);

private:
    TCB(Body body, void* arg, uint64* stack_space,  uint64 timeSlice) :
            body(body),
            stack(body != nullptr ? new uint64[STACK_SIZE]: nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            timeSlice(timeSlice),
            arg(arg),
            finished(false),
            blocked(false),
            sleeping(false),
            semSleepValue(0)
    {
        if(body!=nullptr && body!=Thread::threadWrapper) Scheduler::put(this);
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    void* arg;
    bool finished;
    bool blocked;
    bool sleeping;
    int semSleepValue;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = DEFAULT_STACK_SIZE;
    static uint64 constexpr TIME_SLICE = DEFAULT_TIME_SLICE;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
