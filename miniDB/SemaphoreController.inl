#include "SemaphoreController.hpp"

SemaphoreController::SemaphoreController(int _key)
{
    key = _key;
    sem_id = semget(key, 1, IPC_CREAT|0666);
    if(sem_id == -1)
    {
        printf("SemphoreController -- Constructor Failure\n");
    }
}

void SemaphoreController::reset()
{
    if(semctl(sem_id, 0, SETVAL, 1) == -1)
    {
        printf("SemaphoreController -- Reset Failure\n");
    }
}

void SemaphoreController::erase()
{
    if (semctl(sem_id, 0, IPC_RMID) != 0) 
    {
        printf("SemaphoreController -- Erase Failure\n");
    }
}

void SemaphoreController::lock()
{
    struct sembuf sem_op;

    // setting the flag 
    sem_op.sem_flg = 0;
    
    // "-1" = lock and "+1" = unlock
    sem_op.sem_op = -1;

    // this is the index
    sem_op.sem_num = 0;

    if(semop(sem_id, &sem_op, 1) == -1)
    {
        printf("SemaphoreController -- Lock Failure\n");
    }
}

void SemaphoreController::unlock()
{
    struct sembuf sem_op;

    // setting the flag 
    sem_op.sem_flg = 0;
    
    // "-1" = lock and "+1" = unlock
    sem_op.sem_op = +1;

    // this is the index
    sem_op.sem_num = 0;

    if(semop(sem_id, &sem_op, 1) == -1)
    {
        printf("SemaphoreController -- Unlock Failure\n");
    }
}