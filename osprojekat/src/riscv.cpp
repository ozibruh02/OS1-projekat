
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.h"
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/_sem.h"

using Body = void (*)(void*);
ListOfSleepingThreads Riscv::sleepingThreads;
List<_sem> Riscv::semaphores;
Buffer* Riscv::getBuffer = nullptr;
Buffer* Riscv::putBuffer = nullptr;
bool Riscv::userMode = false;

void Riscv::popSppSpie()
{
    if (userMode) {
        mc_sstatus(Riscv::SSTATUS_SPP);
    }
    else {
        ms_sstatus(Riscv::SSTATUS_SPP);
    }
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        /*TCB::timeSliceCounter = 0;
        TCB::dispatch();*/
        uint64 operation;
        int retValue;
        __asm__ volatile("mv %0, a0" : "=r"(operation));
        switch(operation) {
            case 0x01:
                //mem_alloc
                size_t size;
                void* p;
                __asm__ volatile("mv %0, a1" : "=r"(size));
                p=MemoryAllocator::memm_alloc(size);
                __asm__ volatile("mv t0, %0" : : "r"(p));
                __asm__ volatile ("sw t0, 80(fp)");
                break;
            case 0x02:
                //mem_free
                void *ptr;
                __asm__ volatile("mv %0, a1" : "=r"(ptr));
                retValue = MemoryAllocator::memm_free(ptr);
                __asm__ volatile("mv t0, %0" : : "r"(retValue));
                __asm__ volatile ("sw t0, 80(fp)");
                break;
            case 0x11:
                //thread_create
                TCB** tcb;
                Body body;
                void* arg;
                uint64* stack_space;
                __asm__ volatile("mv %0, a1" : "=r"(tcb));
                __asm__ volatile("mv %0, a2" : "=r"(body));
                __asm__ volatile("mv %0, a3" : "=r"(arg));
                __asm__ volatile("mv %0, a4" : "=r"(stack_space));
                *tcb = TCB::createThread(body,arg,stack_space);
                if(*tcb) {
                    __asm__ volatile("li t0, 0");
                    __asm__ volatile ("sw t0, 80(fp)");
                }
                else {
                    __asm__ volatile("li t0, -1");
                    __asm__ volatile ("sw t0, 80(fp)");
                }
                break;
            case 0x12:
                //thread_exit
                TCB::running->setFinished(true);
                TCB::dispatch();
                __asm__ volatile("li t0, 0");
                __asm__ volatile ("sw t0, 80(fp)");
                break;
            case 0x13:
                //thread_dispatch
                TCB::timeSliceCounter = 0;
                TCB::dispatch();
                break;
            case 0x21:
                //sem_open
                _sem** handle;
                unsigned init;
                __asm__ volatile("mv %0, a1" : "=r"(handle));
                __asm__ volatile("mv %0, a2" : "=r"(init));
                *handle = _sem::createSemaphore(init);
                if(*handle) {
                    semaphores.addLast(*handle);
                    __asm__ volatile("li t0, 0");
                    __asm__ volatile ("sw t0, 80(fp)");
                }
                else {
                    __asm__ volatile("li t0, -1");
                    __asm__ volatile ("sw t0, 80(fp)");
                }
                break;
            case 0x22:
                //sem_close
                _sem* semHandle;
                __asm__ volatile("mv %0, a1" : "=r"(semHandle));
                retValue = semHandle->close();
                __asm__ volatile ("mv t0, %0" : : "r"(retValue));
                __asm__ volatile ("sw t0, 80(fp)");
                break;
            case 0x23:
                //sem_wait
                _sem* id;
                __asm__ volatile("mv %0, a1" : "=r"(id));
                retValue = id->wait();
                __asm__ volatile ("mv t0, %0" : : "r"(retValue));
                __asm__ volatile ("sw t0, 80(fp)");
                break;
            case 0x24:
                //sem_signal
                _sem* id1;
                __asm__ volatile("mv %0, a1" : "=r"(id1));
                retValue = id1->signal();
                __asm__ volatile ("mv t0, %0" : : "r"(retValue));
                __asm__ volatile ("sw t0, 80(fp)");
                break;
            case 0x25:
                //sem_timedwait
                _sem* id2;
                time_t timeout;
                __asm__ volatile("mv %0, a1" : "=r"(id2));
                __asm__ volatile("mv %0, a2" : "=r"(timeout));
                retValue = id2->timedWait(timeout);
                __asm__ volatile ("mv t0, %0" : : "r"(retValue));
                __asm__ volatile ("sw t0, 80(fp)");
                break;
            case 0x26:
                //sem_trywait
                _sem* id3;
                __asm__ volatile("mv %0, a1" : "=r"(id3));
                if(id3->getValue() > 0) {
                    retValue = id3->wait();
                    if(retValue == 0) retValue = 1;
                }
                else retValue = 0;
                __asm__ volatile ("mv t0, %0" : : "r"(retValue));
                __asm__ volatile ("sw t0, 80(fp)");
                break;
            case 0x31:
                //time_sleep
                time_t timeSlice;
                __asm__ volatile("mv %0, a1" : "=r"(timeSlice));
                if(timeSlice!=0) {
                    TCB::running->setSleeping(true);
                    Riscv::sleepingThreads.put(TCB::running,timeSlice);
                }
                TCB::dispatch();
                __asm__ volatile("li t0, 0");
                __asm__ volatile ("sw t0, 80(fp)");
                break;
            case 0x41:
                //getc
                char c1;
                c1 = getBuffer->getc();
                __asm__ volatile ("mv t0, %0" : : "r"(c1));
                __asm__ volatile ("sw t0, 80(fp)");
                break;
            case 0x42:
                //putc
                char c2;
                __asm__ volatile("mv %0, a1" : "=r"(c2));
                putBuffer->putc(c2);
                break;
            default:
                TCB::timeSliceCounter = 0;
                TCB::dispatch();
                break;
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        uint64 volatile sepc = r_sepc();
        uint64 volatile sstatus = r_sstatus();
        time_t currTime = Riscv::sleepingThreads.peekFirstTime();
        time_t tempTime = -1;
        if(currTime != tempTime) {
            Riscv::sleepingThreads.decFirst();
            if(Riscv::sleepingThreads.peekFirstTime() == 0) Riscv::sleepingThreads.removeFinished();
        }
        for(semaphores.setFirst(); semaphores.notEnd(); semaphores.next()) {
            _sem* temp = semaphores.peekCurr();
            if(!temp->closed) {
                time_t currTime = temp->timeBlockedThreads.peekFirstTime();
                if(currTime != tempTime) {
                    temp->timeBlockedThreads.decFirst();
                    if(temp->timeBlockedThreads.peekFirstTime() == 0) temp->timeBlockedThreads.removeFinished();
                }
            }
        }

        mc_sip(SIP_SSIP);
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000009UL)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        uint64 volatile sepc = r_sepc();
        uint64 volatile sstatus = r_sstatus();
        int irq = plic_claim();
		if(irq == CONSOLE_IRQ){
			while (*((char*)(CONSOLE_STATUS)) & CONSOLE_RX_STATUS_BIT) {

				char c = (*(char*)CONSOLE_RX_DATA);
				getBuffer->putc(c);

			}
		}
        plic_complete(irq);
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else
    {
        printString2("Unexpected scause:");
        printInteger2(scause);
        printString2("\n");
    }
}