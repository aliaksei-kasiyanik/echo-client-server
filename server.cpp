#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "echo.h"

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
    int val;                  /* value for SETVAL */
    struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array;    /* array for GETALL, SETALL */
    /* Linux specific part: */
    struct seminfo *__buf;    /* buffer for IPC_INFO */
};
#endif

int main() {

    char *message;

    //create a key for shared memory
    key_t sh_mem_key = ftok(SH_M_PATH, SH_M_PROJ_ID);
    if (sh_mem_key < 0) {
        perror("ftok");
        exit(1);
    }

    // request a shared memory segment
    int sh_mem_id = shmget(sh_mem_key, SH_M_SIZE, 0666);
    if (sh_mem_id < 0) {
        perror("shmget");
        exit(1);
    }

    // attach the indicated shared memory to the process's address space
    void *sh_mem_ptr = shmat(sh_mem_id, NULL, 0);
    if ((int *) sh_mem_ptr < 0) {
        perror("shmat");
        exit(1);
    }

    key_t sem_key = ftok(SEM_PATH, SEM_PROJ_ID);
    if (sem_key < 0) {
        perror("ftok");
        exit(1);
    }

    int sem_id = semget(sem_key, 2, 0666);
    if (sem_key < 0) {
        perror("semget");
        exit(1);
    }

    union semun sem_union;
    sem_union.val = 1;

    // init first semaphore - for communication with server
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) {
        perror("semctl:init:0");
        exit(1);
    }
    // init second semaphore - for clients communication
    if (semctl(sem_id, 1, SETVAL, sem_union) == -1) {
        perror("semctl:init:1");
        exit(1);
    }

    message = (char *) sh_mem_ptr;

    struct sembuf sem_buff;

    printf("Server is running...\n");

    while (true) {

        //lock sem
        sem_buff = {0, -1, 0};
        if (semop(sem_id, &sem_buff, 1) == -1) {
            perror("semop");
            exit(1);
        }

        if (strcmp(message, "")) {
            printf("ECHO: %s", message);
            strcpy(message, "");
        }

        //unlock sem
        sem_buff = {0, 1, 0};
        if (semop(sem_id, &sem_buff, 1) == -1) {
            perror("semop");
            exit(1);
        }
        usleep(1);
    }

    return 0;
}