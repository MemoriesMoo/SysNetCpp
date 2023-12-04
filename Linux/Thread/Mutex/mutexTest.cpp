#include "thread.hpp"
#include <memory>
#include <unistd.h>
#include <vector>

#define NUM 4

// （全局锁不需要初始化销毁）
// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

class ThreadData
{
public:
    std::string thread_name;
    pthread_mutex_t *mutex_p;

public:
    ThreadData(const std::string &name, pthread_mutex_t *mutex) : thread_name(name), mutex_p(mutex) {}
    ~ThreadData() {}
};

// 解决方案：加锁！
// 多个执行流进行安全访问的共享资源 -- 临界资源
// 访问临界资源的代码 -- 临界区（往往是线程代码中的小部分）
// 要实现多个线程串行访问临界资源 -- 互斥
// 对临界资源进行访问 -- 原子性

// 全局共享变量 共享资源
int tickets = 1000;

void *get_tickets(void *args)
{
    // std::string user_name = static_cast<const char *>(args);
    ThreadData *td = static_cast<ThreadData *>(args);
    while (true)
    {
        pthread_mutex_lock(td->mutex_p);
        if (tickets > 0)
        {
            // 为了模拟数据不一致问题 进判断之后先休眠
            // user4正在抢票 剩余票量 : 0
            // user3正在抢票 剩余票量:-1
            // user5正在抢票 剩余票量:-2
            // user1正在抢票 剩余票量:-3
            // user2正在抢票 剩余票量:-4
            // sleep(2);
            
            std::cout << td->thread_name << "正在抢票 "
                      << "剩余票量:" << --tickets << std::endl;

            if (tickets == 0)
                std::cout << "票已售罄！" << std::endl;

            pthread_mutex_unlock(td->mutex_p);
            // 模拟抢票后的其他操作(如生成订单) 防止单个线程无线抢票
            usleep(5000);
        }
        else
        {
            pthread_mutex_unlock(td->mutex_p);
            break;
        }
    }
    return nullptr;
}

int main()
{
    // 初始化锁
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, nullptr);
    // 锁也是共享（临界）资源是否需要被保护呢？
    // pthread_mutex_lock pthread_mutex_unlock 
    // 加锁（原子操作）解锁动作得是安全的！ 
    // 申请锁成功执行流正常向后执行 
    // 暂时未申请成功执行流则阻塞（锁在被其他执行流使用）
    // 谁持有锁则进入临界区执行
    // pthread_mutex_trylock（非阻塞方式）不能上锁则直接出错返回

    // // 测试Thread类
    // std::unique_ptr<Thread> thread0(new Thread(get_tickets, (void *)"user1", 0));
    // std::unique_ptr<Thread> thread1(new Thread(get_tickets, (void *)"user2", 1));
    // std::unique_ptr<Thread> thread2(new Thread(get_tickets, (void *)"user3", 2));
    // std::unique_ptr<Thread> thread3(new Thread(get_tickets, (void *)"user4", 3));
    // std::unique_ptr<Thread> thread4(new Thread(get_tickets, (void *)"user5", 4));

    // thread0->start();
    // thread1->start();
    // thread2->start();
    // thread3->start();
    // thread4->start();

    // thread0->join();
    // thread1->join();
    // thread2->join();
    // thread3->join();
    // thread4->join();
    std::vector<pthread_t> tids(NUM);
    for (int i = 0; i < 4; ++i)
    {
        char nameBuffer[64]{0};
        snprintf(nameBuffer, sizeof(nameBuffer) - 1, "user%d", i + 1);

        // 创建线程数据对象
        ThreadData *td = new ThreadData(nameBuffer, &lock);
        // 将td传入 方便获取名称参数以及lock
        pthread_create(&tids[i], nullptr, get_tickets, td);
    }

    for (const auto &iter : tids)
    {
        // 线程等待
        pthread_join(iter, nullptr);
    }

    // 销毁锁
    pthread_mutex_destroy(&lock);
}