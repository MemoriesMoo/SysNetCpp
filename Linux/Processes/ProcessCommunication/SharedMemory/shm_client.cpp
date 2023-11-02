#include "comm.hpp"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int main()
{
    // Get the shared memory key
    key_t key = getKey();
    printf("key:%d\n", key);

    // Obtain or create a shared memory segment
    int shmId = getShm(key);
    printf("shmId:%d\n", shmId);

    // Attach to the shared memory segment
    char *start = (char *)attachShm(shmId);
    printf("Attach successfully! Address start: %p.\n", start);

    int messageCounter = 0;

    const char *message = "Hello server!";
    pid_t id = getpid();
    int cnt = 0;
    while (true)
    {
        // Write a message to the shared memory segment
        snprintf(start, MAX_SIZE, "%s[pid:%d][message id:%d]", message, id, cnt);
        sleep(1); // Sleep for a while before sending the next message
        cnt++;
    }

    // Detach from the shared memory segment
    detachShm(start);
    printf("Detach successfully!\n");

    exit(0);
}
