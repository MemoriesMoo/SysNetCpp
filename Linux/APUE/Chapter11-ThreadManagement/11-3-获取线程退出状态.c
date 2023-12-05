#include "apue.h"
#include <pthread.h>

// 线程函数1
void *thr_fn1(void *arg)
{
    printf("线程1返回\n");
    return ((void *)1);
}

// 线程函数2
void *thr_fn2(void *arg)
{
    printf("线程2退出\n");
    pthread_exit((void *)2);
}

int main(void)
{
    int err;
    pthread_t tid1, tid2;
    void *tret;

    // 创建线程1
    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if (err != 0)
        err_exit(err, "无法创建线程1");

    // 创建线程2
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if (err != 0)
        err_exit(err, "无法创建线程2");

    // 等待线程1结束
    err = pthread_join(tid1, &tret);
    if (err != 0)
        err_exit(err, "无法与线程1连接");

    printf("线程1退出代码 %ld\n", (long)tret);

    // 等待线程2结束
    err = pthread_join(tid2, &tret);
    if (err != 0)
        err_exit(err, "无法与线程2连接");

    printf("线程2退出代码 %ld\n", (long)tret);
    exit(0);
}
