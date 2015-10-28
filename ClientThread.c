#include "ClientThread.h"

char SPECIAL[] = "Especial";
char REGULAR[] = "Regular";
char NO_CHAIR_FOUND[] = "Cliente %s %d no encuentra silla disponible y se retira. Hora: %s \n";
char SP_CLIENT_QUEUE_OUT[] = "Cliente %s %d no permite la ejecucion de cliente normal, este se retira. Hora: %s \n";
char CHAIR_FOUND[] = "Cliente %s %d logra encontrar silla disponible. Hora: %s \n";
char MOVED_TO_BARBER[] = "Cliente %s %d va ser atendido por un barbero. Hora: %s \n";
char EXEC_CASHIER[] = "Cliente %s %d ya fue atentido por el cajero. Ahora se retira. Hora: %s \n";

char CHAIR_MOVE[] = "Cliente %s %d se mueve a la silla %d en la cola. Hora: %s \n";
char EXEC_BARBER[] = "Cliente %s %d ya fue atentido por el barbero %d. Hora: %s \n";
char CASHIER_MOVE[] = "Cliente %s %d se mueve a la posicion %d del cajero. Hora: %s \n";



/// <summary>
/// Method to allocate memory for a single client
/// </summary>
ClientThread *createClient (
    int pId,
    int *pSpecialClientsCounterPtr,
    int pChairsQueueSize,
    int pBarbersListSize,
    int pCashierQueueSize,
    bool pHasPriority,
    ClientThreadList *pList,
    int *pChairsQueue,
    int *pBarbersList,
    int *pCashiersQueue,
    Semaphore *pChairsSem,
    Semaphore *pBarbersSem,
    Semaphore *pCashierSem,
    Semaphore *pFileSem,
    Semaphore *pSClientsCounterSem)
{
    // Initialize client
    ClientThread *client = malloc(sizeof(ClientThread));

    // Set client props
    client->id = pId;
    client->specialClientsCounterPtr = pSpecialClientsCounterPtr;
    client->chairsQueuSize = pChairsQueueSize;
    client->barbersListSize = pBarbersListSize;
    client->cashierQueueSize = pCashierQueueSize;
    client->isActive = true;
    client->state = 1;
    client->hasPriority = pHasPriority;
    client->actualPosition = -1;
    client->nextClient = NULL;
    client->chairsQueue = pChairsQueue;
    client->barbersList = pBarbersList;
    client->cashiersQueue = pCashiersQueue;
    client->chairsSem = pChairsSem;
    client->barbersSem = pBarbersSem;
    client->cashierSem = pCashierSem;
    client->fileSem = pFileSem;
    client->sClientsCounterSem = pSClientsCounterSem;

    // Add thread to list
    addNodeToClientThreadList(pList,client);
    printf("Thread: %d:  creado \n", client->id);

    // Initialize client thread
    if(pthread_create(&client->thread, 0, threadRun, client))
    {
		fprintf(stderr, "Error creating thread\n");
	}

    return client;
}

/// <summary>
/// Method to allocate memory for the ClientThreadList structure
/// </summary>
ClientThreadList *createClientTreadList()
{
    ClientThreadList *list = malloc(sizeof(ClientThreadList));
    list->first = list->last = NULL;
    return list;
}

/// <summary>
/// Struct method to add an existing client to a list
/// </summary>
void addNodeToClientThreadList(ClientThreadList *pList, ClientThread *pClient)
{
    if (pList->first == NULL)
    {
        pList->first = pList->last = pClient;
    }
    else
    {
        pList->last->nextClient = pClient;
        pList->last = pClient;
    }
    pList->length += 1;
}

/// <summary>
/// Method to wait all threads to finish!
/// </summary>
void joinThreadList(ClientThreadList* pList)
{
	ClientThread *client = pList->first;
	while(client){
        pthread_join(client->thread,0);
		client = client->nextClient;
	}
}

/// <summary>
/// Client run method
/// </summary>
void *threadRun(void  *threadArg)
{
    ClientThread *client = (ClientThread*)threadArg;

    while (client->isActive)
    {
        switch(client->state)
        {
            case 1 :
                verifyChairPosition(client);
                break;
            case 2 :
                executeBarberLogic(client);
                break;
            case 3 :
              executeCashierLogic(client);
              break;
            default :
              printf("Error en el thread: %d \n", client->id);
        }
    }
    pthread_exit(NULL);
    return NULL;
}

/// <summary>
/// Client method to assign a chair to a thread or move to the barbers list
/// </summary>
void verifyChairPosition(ClientThread *pClient)
{
    // Find the client chair
    if(pClient->actualPosition == -1) assignNewChair(pClient);
    else moveFromChair(pClient);
}

/// <summary>
/// Assigns a chair to the new client
/// </summary>
void assignNewChair(ClientThread *pClient)
{
    int chairIndex = findEmptyChair(pClient);
    if(chairIndex != -1)
    {
        pClient->actualPosition = chairIndex;
        writeLog(100,pClient,CHAIR_FOUND);
    }
    else
    {
        writeLog(100,pClient,NO_CHAIR_FOUND);
        pClient->isActive = false;
    }
}

/// <summary>
/// Moves from one chair to another ultill first chair is found
/// </summary>
void moveFromChair(ClientThread *pClient)
{
    // Special Clients Logic
    if(pClient->hasPriority)
    {
        moveSpecialClient(pClient);
    }
    else
    {
        moveRegularClient(pClient);
    }
}

/// <summary>
/// Applies the regular client move logic
/// </summary>
void moveRegularClient(ClientThread *pClient)
{
    // If client can move to barbers
    if(pClient->actualPosition == 0)
    {
        sem_wait(pClient->sClientsCounterSem->mutex);

        if(moveToBarber(pClient))
        {
            int *specialClientsCounter = pClient->specialClientsCounterPtr;
            *specialClientsCounter = 0;
        }

        sem_post(pClient->sClientsCounterSem->mutex);
    }
    else // Client tries to get another chair
    {
        int nextPosition = pClient->actualPosition - 1;

        sem_wait(pClient->chairsSem->mutex);

        // Move from chair
        if(*(pClient->chairsQueue + nextPosition) == 0)
        {

            *(pClient->chairsQueue + pClient->actualPosition) = 0;
            *(pClient->chairsQueue + nextPosition) = 1;
            pClient->actualPosition = nextPosition;
            writeLongLog(150,pClient,CHAIR_MOVE, pClient->actualPosition);
        }

        sem_post(pClient->chairsSem->mutex);
    }
}

/// <summary>
/// Applies the special client move logic
/// </summary>
void moveSpecialClient(ClientThread *pClient)
{
    bool triedToMove = false;

    sem_wait(pClient->sClientsCounterSem->mutex);
    // Only 3 special clients in a row can move
    if(*pClient->specialClientsCounterPtr < 3)
    {
        triedToMove = true;
        if(moveToBarber(pClient))
        {
            int *specialClientsCounter = pClient->specialClientsCounterPtr;
            *specialClientsCounter += 1;
        }
    }
    sem_post(pClient->sClientsCounterSem->mutex);

    // In this case the special client will stop the progra normal execution so
    // we take it out of the chairs
    if((triedToMove == false) &&
        (pClient->actualPosition == 0))
    {

        sem_wait(pClient->chairsSem->mutex);
        *(pClient->chairsQueue + pClient->actualPosition) = 0;
        sem_post(pClient->chairsSem->mutex);

        pClient->isActive = false;
        writeLog(100,pClient,SP_CLIENT_QUEUE_OUT);
    }
}

/// <summary>
/// Tries to move the client to the barbers list
/// </summary>
bool moveToBarber(ClientThread *pClient)
{
    int emptyBarberIndex = findEmptyBarber(pClient);
    bool isMoved = false;

    if(emptyBarberIndex != -1)
    {
        // Deocoupate chair
        sem_wait(pClient->chairsSem->mutex);
        *(pClient->chairsQueue + pClient->actualPosition) = 0;
        sem_post(pClient->chairsSem->mutex);

        pClient->actualPosition = emptyBarberIndex;
        pClient->state = 2;
        writeLog(100,pClient,MOVED_TO_BARBER);
        isMoved = true;
    }

    return isMoved;
}

/// <summary>
/// Search the chairs queue for an empty chair
/// </summary>
int findEmptyChair(ClientThread *pClient)
{

    int index, size = pClient->chairsQueuSize,*indexPointer = pClient->chairsQueue, chairSpaceIndex = -1;

    sem_wait(pClient->chairsSem->mutex);

    for(index = 0; index < size; index++)
    {
        if (*(indexPointer + index) == 0)
        {
            *(indexPointer + index) = 1;
            chairSpaceIndex = index;
            break;
        }
    }

    sem_post(pClient->chairsSem->mutex);

    return chairSpaceIndex;
}

/// <summary>
/// Looks for an empty barber to assign it to a client
/// </summary>
int findEmptyBarber(ClientThread *pClient)
{

    int index, size = pClient->barbersListSize,*indexPointer = pClient->barbersList, spaceIndex = -1;

    sem_wait(pClient->barbersSem->mutex);

    for(index = 0; index < size; index++)
    {
        if (*(indexPointer + index) == 0)
        {
            *(indexPointer + index) = 1;
            spaceIndex = index;
            break;
        }
    }

    sem_post(pClient->barbersSem->mutex);

    return spaceIndex;
}

/// <summary>
/// Sleeps and leaves the barbers list
/// </summary>
void executeBarberLogic(ClientThread *pClient)
{
    int cashierPosition = findCashierQueueSpace(pClient);

    // If there is space
    if(cashierPosition != -1)
    {
        sleep(generateRandomInRange(5,30));

        sem_wait(pClient->barbersSem->mutex);
        *(pClient->barbersList + pClient->actualPosition) = 0;
        sem_post(pClient->barbersSem->mutex);

        pClient->actualPosition = cashierPosition;
        pClient->state = 3;
        writeLongLog(150,pClient,EXEC_BARBER, pClient->actualPosition);
    }


}

/// <summary>
/// Looks for an space in the cashier queue
/// </summary>
int findCashierQueueSpace(ClientThread *pClient)
{
    int index, size = pClient->cashierQueueSize,*indexPointer = pClient->cashiersQueue, cashierSpaceIndex = -1;

    sem_wait(pClient->cashierSem->mutex);

    for(index = 0; index < size; index++)
    {
        if (*(indexPointer + index) == 0)
        {
            *(indexPointer + index) = 1;
            cashierSpaceIndex = index;
            break;
        }
    }

    sem_post(pClient->cashierSem->mutex);

    return cashierSpaceIndex;
}

/// <summary>
/// Sleeps and leaves the barbers list
/// </summary>
void executeCashierLogic(ClientThread *pClient)
{
    // Client is first in line
    if(pClient->actualPosition == 0)
    {
        payToCahier(pClient);
    }
    else
    {
        moveFromCashierQueue(pClient);
    }
}

/// <summary>
/// Tries to move in the cashiers queue
/// </summary>
void moveFromCashierQueue(ClientThread *pClient)
{
    int nextPosition = pClient->actualPosition - 1;
    bool moved = false;

    sem_wait(pClient->cashierSem->mutex);

    // If next space if free
    if(*(pClient->cashiersQueue + nextPosition) == 0)
    {
        *(pClient->cashiersQueue + pClient->actualPosition) = 0;
        *(pClient->cashiersQueue + nextPosition) = 1;
        pClient->actualPosition = nextPosition;

    }
    sem_post(pClient->cashierSem->mutex);
    if(moved)
    {
        writeLongLog(150,pClient,CASHIER_MOVE,pClient->actualPosition);
    }
}

/// <summary>
/// Frees the space in queue an gets inactive
/// </summary>
void payToCahier(ClientThread *pClient)
{
    sleep(generateRandomInRange(5,30));

    sem_wait(pClient->cashierSem->mutex);
    *(pClient->cashiersQueue + pClient->actualPosition) = 0;
    sem_post(pClient->cashierSem->mutex);

    writeLog(150,pClient,EXEC_CASHIER);
    pClient->isActive = false;
}

/// <summary>
/// Formats a string to be written in the file
/// </summary>
void writeLog(int pBufferSize, ClientThread *pClient, char *pFormat)
{
    char str[pBufferSize];
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    char *clientType;

    if(pClient->hasPriority) clientType = SPECIAL;
    else clientType = REGULAR;

    sprintf(str, pFormat,clientType,pClient->id,asctime (timeinfo));
    str[pBufferSize - 1] = NULL;

    printf(str);
    writeFileAppend(str,pClient->fileSem->mutex);

    return str;
}

/// <summary>
/// Formats the barbers string  message to be written in the file
/// </summary>
void writeLongLog(int pBufferSize, ClientThread *pClient, char *pFormat, int pObject)
{
    char str[pBufferSize];
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    char *clientType;

    if(pClient->hasPriority) clientType = SPECIAL;
    else clientType = REGULAR;

    sprintf(str, pFormat,clientType,pClient->id,pObject,asctime (timeinfo));
    str[pBufferSize - 1] = NULL;

    printf(str);
    writeFileAppend(str,pClient->fileSem->mutex);

    return str;
}






