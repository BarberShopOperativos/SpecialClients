#include "BarberShopStructures.h"

/// <summary>
/// Method to generate random number in a range of values
/// </summary>
int generateRandomInRange(int pMin, int pMax)
{
    srand(time(NULL));
    return (rand() % (pMax+1-pMin))+pMin;
}

/// <summary>
/// Clears the cashiers Queue
/// </summary>
void clearControlArray(int *pPointer, int pSize, int pClearValue)
{
    int *pointer = pPointer;

    int i;
    for ( i = 0; i < pSize; i++ )
    {
        *(pointer + i) = pClearValue;
    }

}
