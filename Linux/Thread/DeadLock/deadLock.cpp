// 死锁<->前提是使用锁<->为何使用锁<->保证临界资源安全<->多线程导致数据不一致
// 多线程大部分资源数据是共享的 容易造成不一致问题
// 使用锁是为了解决数据不一致问题但锁也会导致死锁问题
// 单把锁可以造成死锁吗？可以
// 申请两次锁 | 申请锁不释放但其他线（进）程申请锁
// 多把锁如何造成死锁？
// 自身锁不释放却想申请其他线程占有的锁 其他线程也是如此 会导致死锁

// 死锁四个必要条件
// 互斥条件：一个资源每次只能被一个执行流使用
// 请求与保持条件：一个执行流因请求资源而阻塞时，对已获得的资源保持不放
// 不剥夺条件:一个执行流已获得的资源，在末使用完之前，不能强行剥夺
// 循环等待条件:若干执行流之间形成一种头尾相接的循环等待资源的关系

#include <iostream>
#include <pthread.h>
#include <unistd.h>

// 定义两个互斥锁
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

// Worker1线程函数
void *worker1(void *arg)
{
    // 获取mutex1
    pthread_mutex_lock(&mutex1);
    std::cout << "Worker 1获取到mutex1" << std::endl;

    // 睡眠1秒，模拟一些工作
    sleep(1);

    std::cout << "Worker 1等待mutex2" << std::endl;

    // 获取mutex2
    pthread_mutex_lock(&mutex2);
    std::cout << "Worker 1获取到mutex2" << std::endl;

    // 执行一些工作

    // 释放mutex2
    pthread_mutex_unlock(&mutex2);
    std::cout << "Worker 1释放mutex2" << std::endl;

    // 释放mutex1
    pthread_mutex_unlock(&mutex1);
    std::cout << "Worker 1释放mutex1" << std::endl;

    // 线程结束
    pthread_exit(nullptr);
}

// Worker2线程函数
void *worker2(void *arg)
{
    // 获取mutex2
    pthread_mutex_lock(&mutex2);
    std::cout << "Worker 2获取到mutex2" << std::endl;

    // 睡眠1秒，模拟一些工作
    sleep(1);

    std::cout << "Worker 2等待mutex1" << std::endl;

    // 获取mutex1
    pthread_mutex_lock(&mutex1);
    std::cout << "Worker 2获取到mutex1" << std::endl;

    // 执行一些工作

    // 释放mutex1
    pthread_mutex_unlock(&mutex1);
    std::cout << "Worker 2释放mutex1" << std::endl;

    // 释放mutex2
    pthread_mutex_unlock(&mutex2);
    std::cout << "Worker 2释放mutex2" << std::endl;

    // 线程结束
    pthread_exit(nullptr);
}

int main()
{
    // 创建两个线程
    pthread_t thread1, thread2;

    // 启动线程1
    if (pthread_create(&thread1, nullptr, worker1, nullptr) != 0)
    {
        std::cerr << "无法创建线程1" << std::endl;
        return 1;
    }

    // 启动线程2
    if (pthread_create(&thread2, nullptr, worker2, nullptr) != 0)
    {
        std::cerr << "无法创建线程2" << std::endl;
        return 1;
    }

    // 等待线程1结束
    if (pthread_join(thread1, nullptr) != 0)
    {
        std::cerr << "无法等待线程1结束" << std::endl;
        return 1;
    }

    // 等待线程2结束
    if (pthread_join(thread2, nullptr) != 0)
    {
        std::cerr << "无法等待线程2结束" << std::endl;
        return 1;
    }

    return 0;
}
