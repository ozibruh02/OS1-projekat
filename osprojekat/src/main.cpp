#include "../h/syscall_c.h"
#include "../h/print.hpp"
#include "../h/MemoryAllocator.h"
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"

extern void userMain();
bool userMainFinished = false;

void user_wrapper(void*)
{
     userMain();
     printStringForUser("Usermain finished.\n");
     userMainFinished = true;
}

int main()
{
     MemoryAllocator::initMem();
     Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
     Riscv::getBuffer = new Buffer();
     Riscv::putBuffer = new Buffer();

     TCB* outputThread;
     thread_create(&outputThread,&TCB::outputFunction,nullptr);

     TCB *main;
     thread_create(&main,nullptr,nullptr);
     TCB::running = main;

     Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

     printString2("Initialization finished\n");

     Riscv::setMode(true);
     thread_dispatch();

     thread_t user;
     thread_create(&user, user_wrapper, nullptr);

     while(!userMainFinished) thread_dispatch();

     Riscv::setMode(false);
     thread_dispatch();

     printString2("Main finished\n");

     while (Riscv::putBuffer->getNumOfItems() > 0) {
          thread_dispatch();
     }

     delete outputThread;
     delete user;
     delete Riscv::putBuffer;
     delete Riscv::getBuffer;
     return 0;
}
