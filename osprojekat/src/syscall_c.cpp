#include "../h/syscall_c.h"
#include "../lib/hw.h"

void* mem_alloc(size_t size) {
    size_t newSize;
    if(size%MEM_BLOCK_SIZE != 0) {
        newSize = ((size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE;
    }
    else {
        newSize = size;
    }
    __asm__ volatile("mv a1, %0" : : "r"(newSize));
    __asm__ volatile("li a0,0x01");
    __asm__ volatile("ecall");

    void *value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

int mem_free(void* p) {
    __asm__ volatile("mv a1, %0" : : "r"(p));
    __asm__ volatile("li a0,0x02");
    __asm__ volatile("ecall");

    int value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    uint64* stack_space = new uint64[DEFAULT_STACK_SIZE];
    __asm__ volatile("mv a4, %0" : : "r"(stack_space));
    __asm__ volatile("mv a3, %0" : : "r"(arg));
    __asm__ volatile("mv a2, %0" : : "r"(start_routine));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x11");
    __asm__ volatile("ecall");

    int value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

int thread_exit() {
    __asm__ volatile("li a0, 0x12");
    __asm__ volatile("ecall");
    int value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

void thread_dispatch() {
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
}

int sem_open(sem_t* handle, unsigned init) {
    __asm__ volatile("mv a2, %0" : : "r"(init));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x21");
    __asm__ volatile("ecall");
    int value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

int sem_close(sem_t handle) {
    if(!handle) return -1;
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x22");
    __asm__ volatile("ecall");
    int value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

int sem_wait(sem_t id) {
    if(!id) return -1;
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x23");
    __asm__ volatile("ecall");
    int value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

int sem_signal(sem_t id) {
    if(!id) return -1;
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x24");
    __asm__ volatile("ecall");
    int value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

int sem_timedwait(sem_t id, time_t timeout) {
    if(!id) return -3;
    __asm__ volatile("mv a2, %0" : : "r"(timeout));
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x25");
    __asm__ volatile("ecall");
    int value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

int sem_trywait(sem_t id) {
    if(!id) return -1;
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x26");
    __asm__ volatile("ecall");
    int value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

int time_sleep(time_t timeSlice) {
    __asm__ volatile("mv a1, %0" : : "r"(timeSlice));
    __asm__ volatile("li a0, 0x31");
    __asm__ volatile("ecall");
    int value;
    __asm__ volatile("mv %0, a0" : "=r"(value));
    return value;
}

char getc() {
    __asm__ volatile("li a0, 0x41");
    __asm__ volatile("ecall");
    char c;
    __asm__ volatile("mv %0, a0" : "=r"(c));
    return c;
}

void putc(char c) {
    __asm__ volatile("mv a1, %0" : : "r"(c));
    __asm__ volatile("li a0, 0x42");
    __asm__ volatile("ecall");
}


