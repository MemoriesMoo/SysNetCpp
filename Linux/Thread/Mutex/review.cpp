#include <iostream>
#include <pthread.h> // POSIX原生线程库
#include <string>
#include <unistd.h>
#include <cstring>
#include <cerrno>

// 全局变量 线程局部存储（每个线程都有一份 | 互不影响 | 地址不同）
__thread int global_val = 10;

std::string changeId(const pthread_t &tid)
{
    char idBuffer[64]{0};
    snprintf(idBuffer, sizeof(idBuffer) - 1, "0x%x", tid);
    return idBuffer;
}

void *startRoutine(void *args)
{
    std::string threadName = static_cast<const char *>(args);
    // 线程自身进行线程分离
    // pthread_detach(pthread_self());
    int cnt = 5;
    while (cnt--)
    {
        std::cout << threadName << " running..." << changeId(pthread_self()) << std::endl;
        sleep(1);
    }
}

int main()
{
    pthread_t tid;
    if (pthread_create(&tid, nullptr, startRoutine, (void *)"线程1") == 0)
    {
        std::cout << "线程创建成功!" << std::endl;
        pthread_detach(tid);
    }
    else
    {
        std::cerr << "线程创建失败!" << std::endl;
        return 1;
    }

    // 若等待操作在分离之前，分离无效
    // 线程分离之后主线程无需等待因此返回值也是意义的，资源会自动回收
    // int res = pthread_join(tid, nullptr);
    // std::cout << res << ":" << strerror(res) << std::endl;
    int cnt = 5;
    while (cnt--)
    {
        std::cout << "Main thread is running... New thread tid is " << changeId(tid) << std::endl;
        sleep(1);
    }
    return 0;
}

// 每个线程（轻量级进程）都有自己独立的栈结构 如何理解？
// mmap区域 | 动态库 |