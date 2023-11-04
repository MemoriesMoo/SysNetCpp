#include <iostream>
#include <pthread.h>
#include <string>
#include <cstdlib>
#include <unistd.h>

#define TICKET_NUM 1000

// 车票张数（共享资源）
int ticket_num = TICKET_NUM;

// 创建全局互斥锁
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *get_tickets(void *args)
{
    // 转换类型获取参数（此处举例为线程名）
    std::string thread_name = static_cast<const char *>(args);

    while (true)
    {
        // 加锁以保护共享资源
        pthread_mutex_lock(&lock);

        // 检查剩余车票
        if (ticket_num > 0)
        {
            int remaining_tickets = ticket_num;

            // 模拟抢票花费时间
            usleep(2000);

            // 再次检查剩余车票，以确保没有其他线程在抢夺
            if (ticket_num > 0)
            {
                std::cout << thread_name << " is racing to buy tickets! Remaining tickets: " << remaining_tickets << std::endl;
                --ticket_num; // 抢到车票，减少数量
            }

            // 解锁
            pthread_mutex_unlock(&lock);
        }
        else
        {
            // 解锁并退出循环，车票已售完
            pthread_mutex_unlock(&lock);
            break;
        }

        // 模拟抢到票之后的后续操作
        usleep(2000);
    }
}

int main()
{
    // 创建抢票线程
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, nullptr, get_tickets, (void *)"get_tickets thread 1");
    pthread_create(&t2, nullptr, get_tickets, (void *)"get_tickets thread 2");
    pthread_create(&t3, nullptr, get_tickets, (void *)"get_tickets thread 3");
    pthread_create(&t4, nullptr, get_tickets, (void *)"get_tickets thread 4");

    // 等待线程完成
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);
    pthread_join(t4, nullptr);

    // 销毁互斥锁
    pthread_mutex_destroy(&lock);

    return 0;
}
