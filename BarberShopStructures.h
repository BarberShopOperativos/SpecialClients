#ifndef BARBERSHOPSTRUCTURES_H_INCLUDED
#define BARBERSHOPSTRUCTURES_H_INCLUDED
#endif // BARBERSHOPSTRUCTURES_H_INCLUDED
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

// Struct declaration
typedef struct Node Node;
typedef struct Container Container;

/// <summary>
/// Struct to handle queue or list to be used
/// </summary>
typedef struct Container
{
    struct Node *firstNode, *lastNode;
    int length;
    int maxLenght;
};

/// <summary>
/// Struct to handle queue nodes
/// </summary>
typedef struct Node
{
    int id;
    bool isOcupied;
    struct Node *next, *before;
};

Node *createNode (int pId);
Container *createContainer();
Node *addNodeToListContainer(Container *pContainer, int pId);
void printListContainer(Container *pContainer);
Node *addNodeToQueueContainer(Container *pContainer, int pId);
void removeQueueContainerHead(Container *pContainer);
void printQueueContainer(Container *pContainer);
int generateRandomInRange(int pMin, int pMax);
void addExistingNodeToListContainer(Container *pContainer, Node *pNode);
void addExistingNodeToQueueContainer(Container *pContainer, Node *pNode);
void clearCashierQueue(int *pPointer, int pSize, int pClearValue);
