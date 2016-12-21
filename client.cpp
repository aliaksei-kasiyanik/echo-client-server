#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#define SHMSIZE 100

int main() {

    //create a key for shared memory
    key_t sh_mem_key = ftok(".", 'x');
    if (sh_mem_key < 0) {
        perror("ftok");
        exit(1);
    }

    // request a shared memory segment
    int sh_mem_id = shmget(sh_mem_key, SHMSIZE, 0666);
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

    // detach shared memory segment
    if (shmdt(sh_mem_ptr) != 0) {
        perror("shmat");
        exit(1);
    }

    return 0;
}