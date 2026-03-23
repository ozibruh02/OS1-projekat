

#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H
#include "../lib/hw.h"

struct Block {
        Block* next;
        Block* prev;
        size_t size;
};

class MemoryAllocator
{
public:

        static Block* freeMem;
        static Block* allocMem;

        static void initMem();
        static void* memm_alloc (size_t size);
        static int memm_free(void* p);
        static void mergeBlocks(Block* curr);
};



#endif
