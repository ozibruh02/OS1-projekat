#include "../h/MemoryAllocator.h"
#include "../lib/hw.h"
Block* MemoryAllocator::allocMem = nullptr;
Block* MemoryAllocator::freeMem = nullptr;

void MemoryAllocator::initMem() {
    freeMem = (Block*)((char*)HEAP_START_ADDR);
    freeMem->prev = nullptr;
    freeMem->next = nullptr;
    freeMem->size = ((char*)(HEAP_END_ADDR) - (char*)(HEAP_START_ADDR) - sizeof(Block));
}

void * MemoryAllocator::memm_alloc(size_t size) {
    size_t newSize = size;

    for(Block* curr = MemoryAllocator::freeMem; curr != nullptr; curr=curr->next) {
        if(curr->size<newSize) continue;
        if(curr->size > newSize) {
            Block *newBlock;
            newBlock = (Block*) ((char*)curr + newSize + sizeof(Block));

            if(curr->prev) curr->prev->next = newBlock;
            else MemoryAllocator::freeMem = newBlock;
            if(curr->next) curr->next->prev = newBlock;
            newBlock->prev = curr->prev;
            newBlock->next = curr->next;

            newBlock->size = curr->size - newSize - sizeof(Block);
            curr->size = newSize;

            if(allocMem == nullptr) {
                allocMem = curr;
                curr->next = nullptr;
                curr->prev = nullptr;
            }
            else if((char*) curr < (char*)allocMem) {
                allocMem->prev = curr;
                curr->prev = nullptr;
                curr->next = allocMem;
                allocMem = curr;
            }
            else {
                Block* currallocMem = allocMem;
                for(; currallocMem->next && (char*)(currallocMem->next) < (char*) curr; currallocMem = currallocMem->next);

                curr->next = currallocMem->next;
                curr->prev = currallocMem;
                if(curr->next) curr->next->prev = curr;
                currallocMem->next = curr;
            }
        }
        else {
            if (curr->prev) curr->prev->next = curr->next;
            else MemoryAllocator::freeMem = curr->next;

            if(curr->next) curr->next->prev = curr->prev;

            if(allocMem == nullptr) {
                allocMem = curr;
                curr->next = nullptr;
                curr->prev = nullptr;
            }
            else if((char*) curr < (char*)allocMem) {
                allocMem->prev = curr;
                curr->prev = nullptr;
                curr->next = allocMem;
                allocMem = curr;
            }
            else {
                Block* currallocMem = allocMem;
                for(; currallocMem->next && (char*)(currallocMem->next) < (char*) curr; currallocMem = currallocMem->next);

                curr->next = currallocMem->next;
                curr->prev = currallocMem;
                if(curr->next) curr->next->prev = curr;
                currallocMem->next = curr;
            }
        }
        return (char*)curr + sizeof(Block);
    }
    return nullptr;
}

int MemoryAllocator::memm_free(void* p) {
    if(allocMem == nullptr) return -1;
    if(p == nullptr || p < HEAP_START_ADDR || p > HEAP_END_ADDR) return -2;

    Block* curr = (Block*)((char*)p - sizeof(Block));
    if(curr < allocMem) return -3;

    if(allocMem == curr) {
        allocMem = curr->next;
        if(allocMem) allocMem->prev = nullptr;
        curr->next = nullptr;
        curr->prev = nullptr;
    } else {
        curr->prev->next = curr->next;
        if(curr->next) curr->next->prev = curr->prev;
        curr->next = nullptr;
        curr->prev = nullptr;
    }

    if (freeMem == nullptr) {
        freeMem = curr;
        curr->next = nullptr;
        curr->prev = nullptr;
    }
    else if((char*)curr < (char*)freeMem) {
        freeMem->prev = curr;
        curr->prev = nullptr;
        curr->next = freeMem;
        freeMem = curr;
        mergeBlocks(freeMem);
    }
    else {
        Block* currFree = freeMem;
        for(currFree = freeMem; currFree->next && (char*)(currFree->next) < (char*) curr; currFree = currFree->next);

        curr->next = currFree->next;
        curr->prev = currFree;
        if(curr->next) curr->next->prev = curr;
        currFree->next = curr;
        mergeBlocks(curr);
        mergeBlocks(currFree);
    }
    return 0;
}

void MemoryAllocator::mergeBlocks(Block* curr) {
    if(curr->next && (char*)curr + sizeof(Block) + curr->size == (char*)curr->next) {
        curr->size +=curr->next->size + sizeof(Block);
        curr->next = curr->next->next;
        if(curr->next) curr->next->prev = curr;
    }
}
