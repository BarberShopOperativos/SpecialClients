#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "ClientThread.h"

#define INT_SEG_SIZE 5
#define STRUCT_SEG_SIZE 10
#define CASHIER_QUEUE_SIZE 100

char CHAIRS_SEM[] = "ChairsSem";
char BARBERS_SEM[] = "BarbersSem";
char CASHIER_SEM[] = "CashierSem";
char FILE_SEM[] = "FileSem";
char S_CLIENTS_COUNTER_SEM[] = "SpecialClientsCounter";
char CLIENTS_FINISH[] = "Se detuvo el programa generador de clientes /n";

void programInit();
int locateSegment(key_t pKey, int pSegmentSize);
int *pointIntSegment(int pShmID);

int main()
{
    programInit();
    return 0;
}


void programInit()
{
    // Declare needed variables
    int chairsShmID, barbersShmID, cashiershmID, specialClientsCounterShmID,stopClientsShmID,stopSpecialClientesShmID,
        chairsQuantityShmID, barbersQuantityShmID,
        *specialClientsCounterPtr, *stopClientesPtr, *stopSpecialClientsPtr, *chairsQuantityPtr, *barbersQuantityPtr,
        *chairsQueue, *barbersList,*cashierQueue;
    key_t chairsKey, barbersKey, cashierKey, specialClientsCounterKey,stopClientsKey,
        stopSpecialClientsKey,chairsQuantityKey,barbersQuantityKey;

    Semaphore *chairsSem = getSemaphore(CHAIRS_SEM);
    Semaphore *barbersSem = getSemaphore(BARBERS_SEM);
    Semaphore *cashierSem = getSemaphore(CASHIER_SEM);
    Semaphore *fileSem = getSemaphore(FILE_SEM);
    Semaphore *sClientsCounterSem = getSemaphore(S_CLIENTS_COUNTER_SEM);

    // Segment Keys
    chairsKey = 5677;
    barbersKey = 5678;
    cashierKey = 5679;
    specialClientsCounterKey = 5680;
    stopClientsKey = 5681;
    stopSpecialClientsKey = 5682;
    chairsQuantityKey = 5683;
    barbersQuantityKey = 5684;

    // Locate Structures Segments
    chairsQuantityShmID = locateSegment(chairsQuantityKey,INT_SEG_SIZE);
    barbersQuantityShmID = locateSegment(barbersQuantityKey,INT_SEG_SIZE);
    chairsShmID = locateSegment(chairsKey,STRUCT_SEG_SIZE );
    barbersShmID = locateSegment(barbersKey,STRUCT_SEG_SIZE);
    cashiershmID = locateSegment(cashierKey,CASHIER_QUEUE_SIZE);
    specialClientsCounterShmID = locateSegment(specialClientsCounterKey,INT_SEG_SIZE);
    stopClientsShmID = locateSegment(stopClientsKey,INT_SEG_SIZE);
    stopSpecialClientesShmID = locateSegment(stopSpecialClientsKey,INT_SEG_SIZE);

    // Get the structures pointers
    chairsQuantityPtr = pointIntSegment(chairsQuantityShmID);
    barbersQuantityPtr = pointIntSegment(barbersQuantityShmID);
    chairsQueue = pointIntSegment(chairsShmID);
    barbersList = pointIntSegment(barbersShmID);
    cashierQueue = pointIntSegment(cashiershmID);
    specialClientsCounterPtr = pointIntSegment(specialClientsCounterShmID);
    stopClientesPtr = pointIntSegment(stopClientsShmID);
    stopSpecialClientsPtr = pointIntSegment(stopSpecialClientesShmID);

    printf("Cantidad sillas: %d ... \n", *chairsQuantityPtr);
    printf("Cantidad barberos: %d ... \n", *barbersQuantityPtr);
    printf("Special clients counter: %d \n", *specialClientsCounterPtr);


    ClientThreadList *list = createClientTreadList();
    printf("Creando Clientes... \n");

    while(*stopClientesPtr == 1)
    {
        createClient(list->length,specialClientsCounterPtr,*chairsQuantityPtr,*barbersQuantityPtr,
            CASHIER_QUEUE_SIZE,true,list,chairsQueue,barbersList,cashierQueue,
            chairsSem,barbersSem,cashierSem,fileSem,sClientsCounterSem);
        sleep(generateRandomInRange(2,3));
    }

    joinThreadList(list);
    writeFileAppend(CLIENTS_FINISH,fileSem->mutex);

}


/// <summary>
/// Locates a shared memory segment
/// </summary>
int locateSegment(key_t pKey, int pSegmentSize)
{
    int shmID;

    if ((shmID = shmget(pKey, pSegmentSize, 0666)) < 0) {
        printf("Error localizando segmento con llave: %d /n",pKey);
        exit(1);
    }

    //printf("Segment located ... \n");
    return shmID;
}

/// <summary>
/// Gets the pointer to an int value segment
/// </summary>
int *pointIntSegment(int pShmID)
{

    int *pointer;

    if ((pointer = shmat(pShmID, NULL, 0)) == (int *) -1) {
        printf("Error obteniendo puntero de segmento con llave: %d /n", pShmID);
        exit(1);
    }

    //printf("Segment pointed ... \n");
    return pointer;
}
