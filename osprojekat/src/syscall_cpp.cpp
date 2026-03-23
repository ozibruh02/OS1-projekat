//
// Created by os on 6/22/24.
//

#include "../h/syscall_cpp.hpp"
#include "../h/riscv.hpp"

using size_t = decltype(sizeof(0));

void *operator new(size_t n)
{
    return mem_alloc(n);
}

void *operator new[](size_t n)
{
    return mem_alloc(n);
}

void operator delete(void *p)
{
    mem_free(p);
}

void operator delete[](void *p)
{
    mem_free(p);
}

Thread::Thread(void (*body)(void *), void *arg) {
    myHandle = nullptr;
	this->body = body;
    this->arg = arg;
    thread_create(&myHandle, threadWrapper, (void*)this);
}

Thread::~Thread() {
    myHandle->setFinished(true);
    delete myHandle;
}

int Thread::start() {
    myHandle->start();
    return 0;
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    time_sleep(time);
    return 0;
}

void Thread::threadWrapper(void *thread) {
    Thread *threadWrap = (Thread *) thread;
    if (threadWrap->body) {
        threadWrap->body(threadWrap->arg);
    } else {
        threadWrap->run();
    }
}

Thread::Thread() {
    myHandle = nullptr;
	this->body = nullptr;
    this->arg = nullptr;
    thread_create(&myHandle, threadWrapper, (void*)this);
}

Semaphore::Semaphore(unsigned init) {
    myHandle = nullptr;
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t time) {
    return sem_timedwait(myHandle, time);
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}

void PeriodicThread::terminate() {
    this->period = 0;
}

PeriodicThread::PeriodicThread(time_t period) {
    this->period = period;
}

void PeriodicThread::run() {
    while(this->period != 0) {
        this->periodicActivation();
        time_sleep(period);
    }
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}


