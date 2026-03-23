//
// Created by os on 6/21/24.
//

#ifndef BUFFER_H
#define BUFFER_H

#include "_sem.h"

class Buffer {
private:
    _sem* semAvailable;
    char buffer[1024];
    int head = 0, tail = 0, numOfItems = 0;
public:
    Buffer();
    char getc();
    void putc(char c);
    int getNumOfItems() const {return numOfItems;}
};



#endif //BUFFER_H
