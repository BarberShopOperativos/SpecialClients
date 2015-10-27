#ifndef SEMAPHORESTRUCT_H_INCLUDED
#define SEMAPHORESTRUCT_H_INCLUDED

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct Semaphore Semaphore;

/// <summary>
/// Struct to handle the shared semaphores
/// </summary>
struct Semaphore
{
    sem_t *mutex;
    char *name;
};

Semaphore *createSemaphore (char *pName);
Semaphore *getSemaphore(char *pName);
#endif // SEMAPHORESTRUCT_H_INCLUDED
