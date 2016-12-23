#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

#include "echo.h"

int main() {
    key_t sh_mem_key = ftok(SH_M_PATH, SH_M_PROJ_ID);
    if (sh_mem_key < 0) {
        perror("ftok");
        exit(1);
    }

    // request a shared memory segment
    int sh_mem_id = shmget(sh_mem_key, SH_M_SIZE, 0666 | IPC_CREAT);
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

    int sem_id = semget(sem_key, 2, 0666 | IPC_CREAT);
    if (sem_id < 0) {
        perror("semget");
        exit(1);
    }

    printf("Semaphores and shared memory are initialized...\n");

    printf("Press ENTER key to clean up resources:\n");
    getchar();

    // remove semaphore
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl:remove");
        exit(1);
    }

    // detach shared memory segment
    if (shmdt(sh_mem_ptr) != 0) {
        perror("shmat");
        exit(1);
    }

    // remove shared memory segment
    if (shmctl(sh_mem_id, IPC_RMID, NULL) < 0) {
        perror("shmctl");
        exit(1);
    }

    printf("Resources are cleaned up.\n");

    return 0;
}
