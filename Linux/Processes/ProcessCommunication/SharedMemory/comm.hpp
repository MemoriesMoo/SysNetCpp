#ifndef _COMM_HPP
#define _COMM_HPP

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>

#define PATH_NAME "." // 用于生成共享内存键的路径名
#define PROJ_ID 0x66  // 用于生成共享内存键的项目标识符
#define MAX_SIZE 4096 // 共享内存段的最大大小

// 获取共享内存键
key_t getKey()
{
    // 使用ftok函数根据路径名和项目标识符生成共享内存键
    key_t key = ftok(PATH_NAME, PROJ_ID);

    if (key < 0)
    {
        // 如果获取键失败，输出错误信息并退出
        std::cerr << "获取共享内存键失败：" << errno << " - " << strerror(errno) << std::endl;
        exit(1);
    }

    return key;
}

// 辅助函数：获取共享内存段的标识符
int getShmHelper(key_t key, int flags)
{
    // 使用shmget函数获取共享内存段的标识符
    int shmId = shmget(key, MAX_SIZE, flags);

    if (shmId < 0)
    {
        // 如果获取共享内存段失败，输出错误信息并退出
        std::cerr << "获取共享内存段标识符失败：" << errno << " - " << strerror(errno) << std::endl;
        exit(2);
    }

    return shmId;
}

// 获取或创建共享内存段
int getShm(key_t key)
{
    // 使用getShmHelper函数获取或创建共享内存段
    return getShmHelper(key, IPC_CREAT /* 0 */);
}

// 创建共享内存段（如果已存在则报错）
int createShm(key_t key)
{
    // 使用getShmHelper函数创建共享内存段，如果已存在则报错
    return getShmHelper(key, IPC_CREAT | IPC_EXCL | 0666);
}

// 删除共享内存段
void delShm(int shmId)
{
    if (shmctl(shmId, IPC_RMID, nullptr) == -1)
    {
        // 如果删除共享内存段失败，输出错误信息并退出
        std::cerr << "删除共享内存段失败：" << errno << " - " << strerror(errno) << std::endl;
        exit(3);
    }
}

// 连接到共享内存段
void *attachShm(int shmId)
{
    // 使用shmat函数连接到共享内存段
    void *mem = shmat(shmId, nullptr, 0);

    if ((long long)mem == -1L)
    {
        // 如果连接失败，输出错误信息并退出
        std::cerr << "连接共享内存段失败：" << errno << " - " << strerror(errno) << std::endl;
        exit(4);
    }

    return mem;
}

// 从共享内存段分离
void detachShm(void *start)
{
    if (shmdt(start) == -1)
    {
        // 如果分离失败，输出错误信息并退出
        std::cout << "分离共享内存段失败：" << errno << " - " << strerror(errno) << std::endl;
        exit(5);
    }
}

#endif
