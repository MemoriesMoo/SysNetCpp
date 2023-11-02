#ifndef _COMM_HPP
#define _COMM_HPP

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>

#define PATH_NAME "."   // Path name for generating the shared memory key
#define PROJ_ID 0x66     // Project identifier for generating the shared memory key
#define MAX_SIZE 4096    // Maximum size of the shared memory segment

// Function to get the key for shared memory
key_t getKey()
{
    key_t key = ftok(PATH_NAME, PROJ_ID);

    if (key < 0)
    {
        // If obtaining the key fails, output error information and exit
        std::cerr << "getKey-> Error: " << errno << ": " << strerror(errno) << std::endl;
        exit(1);
    }

    return key;
}

// Helper function: Get the identifier of a shared memory segment
int getShmHelper(key_t key, int flags)
{
    int shmId = shmget(key, MAX_SIZE, flags);

    if (shmId < 0)
    {
        // If obtaining the shared memory segment fails, output error information and exit
        std::cerr << "getShmHelper-> Error: " << errno << ": " << strerror(errno) << std::endl;
        exit(2);
    }

    return shmId;
}

// Function to get or create a shared memory segment
int getShm(key_t key)
{
    return getShmHelper(key, IPC_CREAT /* 0 */);
}

// Function to create a shared memory segment (error if it already exists)
int createShm(key_t key)
{
    return getShmHelper(key, IPC_CREAT | IPC_EXCL | 0666);
}

// Function to delete a shared memory segment
void delShm(int shmId)
{
    if (shmctl(shmId, IPC_RMID, nullptr) == -1)
    {
        std::cerr << "delShm-> Error: " << errno << ": " << strerror(errno) << std::endl;
        exit(3);
    }
}

// Function to attach to a shared memory segment
void *attachShm(int shmId)
{
    void *mem = shmat(shmId, nullptr, 0);

    if ((long long)mem == -1L)
    {
        std::cerr << "attachShm-> Error: " << errno << ": " << strerror(errno) << std::endl;
        exit(4);
    }

    return mem;
}

// Function to detach from a shared memory segment
void detachShm(void *start)
{
    if (shmdt(start) == -1)
    {
        std::cout << "detachShm-> Error: " << errno << ": " << strerror(errno) << std::endl;
        exit(5);
    }
}

#endif
