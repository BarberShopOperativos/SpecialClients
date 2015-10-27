#ifndef CLIENTTHREAD_H_INCLUDED
#define CLIENTTHREAD_H_INCLUDED


#include <time.h>
#include "BarberShopStructures.h"
#include "SemaphoreStruct.h"

typedef struct ClientThread ClientThread;
typedef struct ClientThreadList ClientThreadList;

/// <summary>
/// Struct to handle the matrix threads
/// </summary>
struct ClientThread
{
    int id;
    int *specialClientsCounterPtr;
    bool isActive;
    bool hasPriority;
    int cashierQueueSize;
    int cashierPosition;
    int state; // State 1 -> Chairs Queue, State 2-> BarbersList, State 3 -> Cashier Queue, State 4 -> Finished
    pthread_t thread;
    Node *actualNode;
    ClientThread *nextClient;
    Container *chairsQueue;
    Container *barbersList;
    int *cashiersQueue;
    Semaphore *chairsSem;
    Semaphore *barbersSem ;
    Semaphore *cashierSem;
    Semaphore *fileSem;
    Semaphore *sClientsCounterSem;
};

/// <summary>
/// Struct to handle queue or list to be used
/// </summary>
typedef struct ClientThreadList
{
    struct ClientThread *first, *last;
    int length;
    int maxLenght;
};

void *threadRun(void * threadArg);
ClientThreadList *createClientTreadList();
void addNodeToClientThreadList(ClientThreadList *pList, ClientThread *pClient);
void joinThreadList(ClientThreadList* pList);
ClientThread *createClient (
    int pId,
    int *pSpecialClientsCounterPtr,
    int pCashierQueueSize,
    bool pHasPriority,
    ClientThreadList *pList,
    Container *pChairsQueue,
    Container *pBarbersList,
    int *pCashiersQueue,
    Semaphore *pChairsSem,
    Semaphore *pBarbersSem,
    Semaphore *pCashierSem,
    Semaphore *pFileSem,
    Semaphore *pSClientsCounterSem);

void verifyChairPosition(ClientThread *pClient);
void assignNewChair(ClientThread *pClient);
void moveFromChair(ClientThread *pClient);
void moveRegularClient(ClientThread *pClient);
void moveSpecialClient(ClientThread *pClient);
bool moveToBarber(ClientThread *pClient);
Node *findEmptyChair(ClientThread *pClient);
Node *findEmptyBarber(ClientThread *pClient);
void executeBarberLogic(ClientThread *pClient);
int findCashierQueueSpace(ClientThread *pClient);
void executeCashierLogic(ClientThread *pClient);
void moveFromCashierQueue(ClientThread *pClient);
void payToCahier(ClientThread *pClient);
void writeLog(int pBufferSize, ClientThread *pClient, char *pFormat);
void writeLongLog(int pBufferSize, ClientThread *pClient, char *pFormat, int pObject);

#endif // CLIENTTHREAD_H_INCLUDED