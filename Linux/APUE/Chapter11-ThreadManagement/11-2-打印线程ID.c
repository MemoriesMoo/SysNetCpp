#include "apue.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t ntid; // 全局变量，存储新创建线程的ID

// 打印进程ID和线程ID的函数
void printids(const char *s)
{
    pid_t pid;
    pthread_t tid;

    pid = getpid();       // 获取进程ID
    tid = pthread_self(); // 获取线程ID

    // 打印进程ID、线程ID、线程ID的十六进制表示
    printf("%s pid %lu tid %lu (0x%x)\n",
           s, (unsigned long)pid, (unsigned long)tid, (unsigned long)tid);
}

// 新线程执行的函数
void *thread_fun(void *arg)
{
    printids("new thread: "); // 在新线程中打印信息
    return ((void *)0);
}

int main()
{
    int err;

    // 创建新线程
    err = pthread_create(&ntid, NULL, thread_fun, NULL);
    if (err != 0)
    {
       err_exit(err, "无法创建线程");
    }

    printids("main thread: "); // 在主线程中打印信息
    sleep(1);                  // 等待一秒钟，以确保新线程有足够的时间执行
    return 0;
}
