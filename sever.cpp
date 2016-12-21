#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

#define SHMSIZE 100

int main() {

    char *message;

    //create a key for shared memory
    key_t sh_mem_key = ftok(".", 'x');
    if (sh_mem_key < 0) {
        perror("ftok");
        exit(1);
    }

    // request a shared memory segment
    int sh_mem_id = shmget(sh_mem_key, SHMSIZE, IPC_CREAT | 0666);
    if (sh_mem_id < 0) {
        perror("shmget");
        exit(1);
    }

    // attach the indicated shared memory to the process's address space
    void *sh_mem_ptr = shmat(sh_mem_id, NULL, 0);
    if ((int) sh_mem_ptr < 0) {
        perror("shmat");
        exit(1);
    }

    key_t sem_key = ftok(".", getpid());
    if (sem_key < 0) {
        perror("ftok");
        exit(1);
    }

    int sem_id = semget(sem_key, 1, IPC_CREAT | 0666);
    if (sem_key < 0) {
        perror("semget");
        exit(1);
    }

    union semun sem_union;
    sem_union.val = 1;

    // init semaphore
    if(semctl(sem_id, 0, SETVAL, sem_union) == -1) {
        perror("semctl:init");
        exit(1);
    }

    // remove semaphore
    if(semctl(sem_id, 0, IPC_RMID, NULL) == -1) {
        perror("semctl:remove");
        exit(1);
    }

    message = (char *) sh_mem_ptr;



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
    return 0;
}