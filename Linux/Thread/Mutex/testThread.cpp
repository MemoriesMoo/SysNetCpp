#include "thread.hpp"
#include <memory>
#include <unistd.h>

void *thread_run(void *args)
{
    std::string work_type = static_cast<const char *>(args);
    while (true)
    {
        std::cout << "我是新线程"
                  << " work_type:" << work_type << std::endl;

        sleep(1);
    }
}

int main()
{
    // 测试Thread类
    std::unique_ptr<Thread> thread0(new Thread(thread_run, (void *)"下载", 0));
    std::unique_ptr<Thread> thread1(new Thread(thread_run, (void *)"播放", 1));
    std::unique_ptr<Thread> thread2(new Thread(thread_run, (void *)"传输", 2));
    thread0->start();
    thread1->start();
    thread2->start();
    thread0->join();
    thread1->join();
    thread2->join();
}