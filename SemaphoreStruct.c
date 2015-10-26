#include "SemaphoreStruct.h"

/// <summary>
/// Method to create a new Semaphore struct
/// </summary>
Semaphore *createSemaphore (char *pName)
{
    Semaphore *semaphore = malloc(sizeof(Semaphore));
    semaphore->name = pName;

    //create & initialize existing semaphore
    semaphore->mutex = sem_open(pName,O_CREAT,0644,1);
    if(semaphore->mutex == SEM_FAILED)
    {
        printf("Error inicializando semaforo: %s \n", pName);
        sem_unlink(pName);
        exit(-1);
    }

    // Just to unlock all the semaphores in use
    sem_post(semaphore->mutex);

    printf("Semaforo %s creado... \n", pName);

    return semaphore;
}

/// <summary>
/// Method to restore a previously created semaphore
/// </summary>
Semaphore *getSemaphore(char *pName)
{
    Semaphore *semaphore = malloc(sizeof(Semaphore));
    semaphore->name = pName;

    //create & initialize existing semaphore
    semaphore->mutex = sem_open(pName,0,0644,0);
    if(semaphore->mutex == SEM_FAILED)
    {
        printf("Error recuperando semaforo: %s \n", pName);
        sem_close(pName);
        exit(-1);
    }

    printf("Semaforo %s recuperado... \n", pName);

    return semaphore;
}
