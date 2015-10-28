#ifndef BARBERSHOPSTRUCTURES_H_INCLUDED
#define BARBERSHOPSTRUCTURES_H_INCLUDED
#endif // BARBERSHOPSTRUCTURES_H_INCLUDED
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

int generateRandomInRange(int pMin, int pMax);
void clearControlArray(int *pPointer, int pSize, int pClearValue);
