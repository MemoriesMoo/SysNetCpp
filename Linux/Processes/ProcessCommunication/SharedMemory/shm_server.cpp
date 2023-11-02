#include "comm.hpp"
#include <stdio.h>
#include <unistd.h>

int main() {
    // Get the shared memory key
    key_t key = getKey();
    printf("key:%d\n", key);

    // Create a new shared memory segment
    int shmId = createShm(key);
    printf("shmId:%d\n", shmId);

    // Attach to the shared memory segment
    char *start = (char *)attachShm(shmId);
    printf("Attach successfully! Address start: %p.\n", start);

    while (true) {
        // Read and print the message from the shared memory
        printf("Message from shared memory: %s\n", start);
        sleep(1); // Sleep for a while before reading the next message
    }

    // Detach from the shared memory segment
    detachShm(start);
    printf("Detach successfully!\n");

    // Delete the shared memory segment
    delShm(shmId);

    exit(0);
}
