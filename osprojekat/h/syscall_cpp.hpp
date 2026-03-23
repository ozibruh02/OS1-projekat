#ifndef _syscall_cpp
#define _syscall_cpp

#include "syscall_c.h"

void* operator new (size_t);
void operator delete (void*);

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t time);
    static void threadWrapper(void *thread);
protected:
    Thread ();
    virtual void run () {}
private:
    thread_t myHandle;
    void (*body)(void*); void* arg;
};

class Semaphore {
public:
    explicit Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
    int timedWait (time_t time);
    int tryWait();
private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
public:
    void terminate ();
protected:
    explicit PeriodicThread (time_t period);
    virtual void periodicActivation () {}
    void run() final;
private:
    time_t period;
};

class Console {
public:
    static char getc ();
    static void putc (char c);
};

#endif