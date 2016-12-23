#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>

#include "echo.h"

int main() {

    //create a key for shared memory
    key_t sh_mem_key = ftok(".", 'x');
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

    int sem_id = semget(sem_key, 2, IPC_CREAT | 0666);
    if (sem_key < 0) {
        perror("semget");
        exit(1);
    }

    struct sembuf sem_buff;

    char *message = (char *) sh_mem_ptr;

    while (true) {
        //lock client
        sem_buff = {1, -1, 0};
        if (semop(sem_id, &sem_buff, 1) == -1) {
            perror("semop");
            exit(1);
        }

        //lock server
        sem_buff = {0, -1, 0};
        if (semop(sem_id, &sem_buff, 1) == -1) {
            perror("semop");
            exit(1);
        }

        printf("Enter text: \n");
        fgets(message, 100, stdin);

        //unlock server
        sem_buff = {0, 1, 0};
        if (semop(sem_id, &sem_buff, 1) == -1) {
            perror("semop");
            exit(1);
        }

        //unlock client
        sem_buff = {1, 1, 0};
        if (semop(sem_id, &sem_buff, 1) == -1) {
            perror("semop");
            exit(1);
        }
        usleep(5);
    }

    return 0;
}