#include <stdio.h>
#include "semaphore.h"

/// <summary>
/// Method to delete and write on a specified file
/// </summary>
void writeFile(char *pText){
    char *fileName = "../output.txt";
    FILE *file = fopen(fileName, "w");
    if (file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(file, "%s", pText);
    fclose(file);
}

/// <summary>
/// Method to append text on a specified file
/// </summary>
void writeFileAppend(char *pText, sem_t *pMutex){

    sem_wait(pMutex);

    char *fileName = "../output.txt";
    FILE *file = fopen(fileName, "a");
    if (file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    //if(ftrylockfile(file) == 0){
        fprintf(file, "%s", pText);
        //funlockfile(file);
        fclose(file);
   // }

   sem_post(pMutex);
}
