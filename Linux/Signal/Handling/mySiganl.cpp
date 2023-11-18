#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>

constexpr int BLOCK_SIG = 2;
constexpr int MAX_SIGNUM = 31;

// 显示挂起信号的函数
void show_pending(const sigset_t &pending)
{
    // 遍历所有信号
    for (int sig = 1; sig <= MAX_SIGNUM; ++sig)
    {
        // 如果信号在挂起集合中，打印1，否则打印0
        std::cout << (sigismember(&pending, sig) ? "1" : "0");
    }
    std::cout << "  (1 表示有信号挂起)" << std::endl;
}

// 处理SIGINT信号的函数
void handle_sigint(int)
{
    std::cout << "接收到 Ctrl+C，信号已经被捕捉！\n";

    // 在这里可以执行一些清理操作，然后退出程序
    // exit(0);
}

int main()
{
    // 注册SIGINT信号的处理函数
    std::signal(SIGINT, handle_sigint);

    // 信号集初始化
    sigset_t block, oblock, pending;
    sigemptyset(&block);   // 清空屏蔽集合
    sigemptyset(&oblock);  // 清空旧屏蔽集合
    sigemptyset(&pending); // 清空挂起集合

    // 添加要屏蔽的信号
    sigaddset(&block, BLOCK_SIG);

    // 屏蔽信号并保存旧的掩码
    if (sigprocmask(SIG_SETMASK, &block, &oblock) == -1)
    {
        perror("sigprocmask"); // 打印错误信息并退出
        return 1;
    }

    std::cout << "屏蔽信号 " << BLOCK_SIG
              << " 并每秒检查是否有信号挂起。按下 Ctrl+C 以退出程序：" << std::endl;

    int cnt = 10;
    while (true)
    {
        // 获取挂起的信号
        sigemptyset(&pending);
        if (sigpending(&pending) == -1)
        {
            perror("sigpending"); // 打印错误信息并退出
            break;
        }

        // 打印挂起的信号
        show_pending(pending);

        // 休眠1秒
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (cnt-- == 0)
        {
            std::cout << "恢复旧的信号掩码" << std::endl;
            // 恢复旧的信号掩码
            if (sigprocmask(SIG_SETMASK, &oblock, nullptr) == -1)
            {
                perror("sigprocmask"); // 打印错误信息并退出
                return 1;
            }
        }
    }

    return 0;
}
