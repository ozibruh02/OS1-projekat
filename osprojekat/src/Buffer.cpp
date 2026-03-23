//
// Created by os on 6/21/24.
//

#include "../h/Buffer.h"
#include "../h/syscall_c.h"

Buffer::Buffer() {
    sem_open(&semAvailable,0);
}

char Buffer::getc() {
    semAvailable->wait();
    char c = buffer[head];
    head = (head+1)%1024;
    numOfItems--;
    return c;
}

void Buffer::putc(char c) {
    if(numOfItems < 1024) {
        buffer[tail] = c;
        tail = (tail+1)%1024;
        numOfItems++;
        semAvailable->signal();
    }
}
