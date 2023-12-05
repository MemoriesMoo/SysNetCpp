#include "apue.h"
#include <pthread.h>

struct foo
{
    int a, b, c, d;
};

// 打印结构体成员的函数
void printfoo(const char *s, const struct foo *fp)
{
    printf("%s", s);
    printf("  结构体地址为 0x%lx\n", (unsigned long)fp);
    printf("  foo.a = %d\n", fp->a);
    printf("  foo.b = %d\n", fp->b);
    printf("  foo.c = %d\n", fp->c);
    printf("  foo.d = %d\n", fp->d);
}

// 线程函数1
void *thr_fn1(void *arg)
{
    struct foo foo = {1, 2, 3, 4};

    printfoo("线程1:\n", &foo);
    // 注意：这里返回的是局部变量的地址，潜在的问题，请勿在实际代码中这样使用
    pthread_exit((void *)&foo);
}

// 线程函数2
void *thr_fn2(void *arg)
{
    printf("线程2: ID为 %lu\n", (unsigned long)pthread_self());
    pthread_exit((void *)0);
}

int main(void)
{
    int err;
    pthread_t tid1, tid2;
    struct foo *fp;

    // 创建线程1
    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if (err != 0)
        err_exit(err, "无法创建线程1");

    // 等待线程1结束，并获取返回值
    err = pthread_join(tid1, (void *)&fp);
    if (err != 0)
        err_exit(err, "无法与线程1连接");

    sleep(1);
    printf("父进程开始第二个线程\n");

    // 创建线程2
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if (err != 0)
        err_exit(err, "无法创建线程2");

    sleep(1);
    printfoo("父进程:\n", fp);

    exit(0);
}
