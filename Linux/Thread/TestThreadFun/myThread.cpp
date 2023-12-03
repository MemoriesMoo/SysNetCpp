#include <iostream>
#include <pthread.h> // POSIX线程库
#include <cassert>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstring>

#define THREAD_NUM 10

// 1.什么资源线程私有？
// PCB属性 线程上下文 栈结构

// 2.线程间切换相较于进程间切换
// 进程切换：切换页表 && 虚拟地址空间 && PCB && 上下文
// 线程切换：PCB && 上下文
// CPU缓存问题：切换进程要重新加载缓存（存储相关热点数据）
// 进程内线程共享缓存数据切换时无需全部重新加载

// 线程相关数据
class ThreadData
{
public:
    pthread_t tid;
    char nameBuffer[64]{0};
};

// 线程返回信息
class ThreadReturn
{
public:
    int exit_code;
    int exit_result;
};

// 新线程执行流函数
// 此函数被多线程调用 处于重入状态
// 是否为可重入函数？（此处忽略输出，可视为可重入函数）
// 局部变量具有临时性 多线程情况下同样适合用 不互相影响 独立栈结构
void *thread_routine(void *args)
{
    ThreadData *td = static_cast<ThreadData *>(args);
    int cnt = 10;
    while (cnt--)
    {
        printf("ThreadName:%s\n", td->nameBuffer);
        sleep(1);
    }

    //   delete td;
    //   不能再此处结束之后就delete td
    //   因为主线程在阻塞式等待需要用到，否则会出现
    //   thread 1等待成功！
    //   Segmentation fault
    //   因为进程还未结束时主线程已经在等待线程1，已经合法获取到td->tid
    //   若线程1等待成功后再去等待线程二则会访问已释放的空间导致段错误

    // exit终止进程无法用来终止线程
    // 任何一个执行流调用exit都会导致进程结束

    // 调用pthread_exit函数终止线程 不影响其他线程
    // ThreadReturn *tr = new ThreadReturn();
    // tr->exit_code = 1;
    // tr->exit_result = 2;
    pthread_exit((void *)100);

    // 直接使用return也可终止线程与调用线程终止函数类似
    // return nullptr;
}

int main()
{
    // 进程数据对象vector
    std::vector<ThreadData *> threads;

    for (int i = 0; i < THREAD_NUM; ++i)
    {
        // 堆对象
        ThreadData *td = new ThreadData();
        // 成员属性赋值
        snprintf(td->nameBuffer, sizeof(td->nameBuffer), "%s %d", "thread", i + 1);
        // 线程创建
        int res = pthread_create(&td->tid, nullptr, thread_routine, td);
        assert(!res);
        (void)res;

        threads.push_back(td);
    }

    sleep(5);
    // 主线程先等待五秒钟 让其他线程先运行起来
    for (int i = 0; i < THREAD_NUM / 2; ++i)
    {
        int res = pthread_cancel(threads[i]->tid);
        assert(!res);
        (void)res;
        std::cout << "取消: " << threads[i]->nameBuffer << std::endl;
    }

    // 主线程进程线程等待
    // 1.获取线程退出信息（选择性接收）
    // 2.回收线程对应的PCB等资源，防止内存泄露
    // 阻塞式等待
    for (auto &iter : threads)
    {
        // void *
        void *ret = nullptr;
        // 输出型参数 void ** 获取线程退出提供的返回值
        int res = pthread_join(iter->tid, (void **)&ret);
        // 确保等待成功
        assert(!res);
        (void)res;

        // std::cout << iter->nameBuffer << "等待成功！"
        //           << "Exit_code:" << ret->exit_code << " "
        //           << "Exit_result:" << ret->exit_result
        //           << std::endl;

        // 调用线程取消函数后，不会按照原先函数内部的逻辑执行结束，而是返回错误码-1->PTHREAD_CANCELED
        // 因此如果依然把返回值当作ThreadReturn*使用并释放会导致段错误
        std::cout << iter->nameBuffer << "等待成功！ Exit_code:"
                  << (long long)ret << std::endl;
        // 释放开辟的对象堆空间
        delete iter;
        // delete ret;
    }

    // 线程退出时信号呢？
    // 线程出异常，收到信号，整个进程都会退出
    // 异常是进程该关心的内容

    std::cout << "线程均等待成功，主线程结束！\n";
}