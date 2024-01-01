#ifndef SEMAPHORE_CONTROLLER_H
#define SEMAPHORE_CONTROLLER_H

#include "Dependencies.hpp"
#include "StatusCodes.hpp"

class SemaphoreController
{
private:
    int key;
    int sem_id;
public:
    // default constructor
    SemaphoreController(){}

    // constructor with semaphore key [complete]
    SemaphoreController(int _key);

    // create the semaphore and make value one [complete]
    void reset();

    // delete the semaphore [complete]
    void erase();

    // semaphore lock abstraction [complete]
    void lock();

    // semaphore unlock abstraction [complete]
    void unlock();
};

#include "SemaphoreController.inl"
#endif // SEMAPHORE_CONTROLLER_H