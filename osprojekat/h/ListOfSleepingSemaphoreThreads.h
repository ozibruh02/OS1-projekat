//
// Created by os on 6/23/24.
//

#ifndef LISTOFSLEEPINGSEMAPHORETHREADS_H
#define LISTOFSLEEPINGSEMAPHORETHREADS_H

#include "ListOfSleepingThreads.h"

class ListOfSleepingSemaphoreThreads: public ListOfSleepingThreads {
public:
    ListOfSleepingSemaphoreThreads(_sem* sem): ListOfSleepingThreads(){Semafor = sem;}

    void removeFinished() override;

    void removeAll();
private:
    _sem* Semafor;
};

#endif //LISTOFSLEEPINGSEMAPHORETHREADS_H
