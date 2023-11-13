#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <functional>
#include <ctime>
#include <random>
#include <csignal>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cassert>
#include <fcntl.h>

#define POOL_NUM 5

typedef void (*func_t)();

// 全局变量存储创建的命名管道
std::vector<std::string> allPipe;

class SubProc
{
public:
    SubProc(pid_t id, int writeFd)
        : _id(id),
          _writeFd(writeFd)
    {
        // 为每个子进程生成一个名字，用于标识
        char buffer[1024]{0};
        snprintf(buffer, sizeof(buffer) - 1, "pool[%d]-[processPid(%d)-writeFd(%d)]",
                 cnt++, _id, writeFd);
        _name = buffer;
    }

public:
    static int cnt;    // 静态计数器，用于生成唯一的子进程名字后缀
    std::string _name; // 子进程的名字
    pid_t _id;         // 子进程的ID
    int _writeFd;      // 子进程的写入文件描述符
};

int SubProc::cnt = 0;

// 全局变量，用于保存进程池
std::vector<SubProc> pool;

// 任务函数，这里简单实现了三个任务函数
void downloadTask()
{
    sleep(1);
    std::cout << "进程[" << getpid() << "] 正在执行下载任务!\n";
}

void ioTask()
{
    sleep(1);
    std::cout << "进程[" << getpid() << "] 正在执行IO任务!\n";
}

void flushTask()
{
    sleep(1);
    std::cout << "进程[" << getpid() << "] 正在执行刷新任务!\n";
}

void exitTask()
{
    std::cout << "进程[" << getpid() << "] 接收到退出任务，准备退出!\n";
    exit(EXIT_SUCCESS);
}

void loadTaskFunc(std::vector<func_t> &funcMap)
{
    funcMap.push_back(downloadTask);
    funcMap.push_back(ioTask);
    funcMap.push_back(flushTask);
    funcMap.push_back(exitTask); // 添加退出任务
}

// 从进程接收任务
int recTask(int readFd)
{
    int code = 0;
    ssize_t readBytes = read(readFd, &code, sizeof(code));
    if (readBytes != sizeof(code))
    {
        std::cerr << "读取任务失败\n";
    }
    return code;
}

// 向进程发送任务
void sendTask(const SubProc &sp, int taskId)
{
    std::cout << "发送任务[" << taskId << "] 到进程 " << sp._name << "\n";

    int res = write(sp._writeFd, &taskId, sizeof(taskId));
    if (res != sizeof(taskId))
    {
        std::cerr << "写入任务失败\n";
        // 可以根据实际需求进行错误处理，这里简单输出错误信息
    }
}

// 创建进程池
void createProcPool(std::vector<SubProc> &pool, std::vector<func_t> &funcMap)
{
    for (int cnt = 0; cnt < POOL_NUM; ++cnt)
    {
        // 此处创建匿名管道替换成命名管道
        // 利用cnt构建管道名称
        std::string namedPipe = "./myPipe" + std::to_string(cnt);
        umask(0);
        int res = mkfifo(namedPipe.c_str(), 0600);
        assert(res == 0);

        pid_t id = fork();
        if (id == 0)
        {

            while (true)
            {
                int readFd = open(namedPipe.c_str(), O_RDONLY);
                assert(readFd >= 0);
                int commandCode = recTask(readFd);
                if (commandCode >= 0 && commandCode < funcMap.size())
                {
                    funcMap[commandCode]();
                }
                else
                {
                    std::cout << "子进程接收到错误的命令码\n";
                }
            }

            exit(EXIT_SUCCESS);
        }

        int writeFd = open(namedPipe.c_str(), O_WRONLY);
        SubProc sub(id, writeFd);
        pool.push_back(std::move(sub));
        allPipe.push_back(namedPipe);
    }
}

// 清理资源，关闭文件描述符
void cleanupResources(const std::vector<SubProc> &pool)
{
    std::cout << "清理资源...\n";
    for (const auto &subProc : pool)
    {
        close(subProc._writeFd);
    }

    for (const auto &pipe : allPipe)
    {
        unlink(pipe.c_str());
    }
}

// 等待子进程退出
void waitChildProcesses(const std::vector<SubProc> &pool)
{
    for (const auto &subProc : pool)
    {
        int status;
        waitpid(subProc._id, &status, 0);
    }
}

// 信号处理函数，捕获Ctrl+C信号(SIGINT)
void signalHandler(int signum)
{
    std::cout << "接收到信号：" << signum << "\n";
    // 等待所有子进程退出
    waitChildProcesses(pool);
    // 清理资源
    cleanupResources(pool);
    exit(signum);
}

int main()
{
    std::vector<func_t> funcMap;
    loadTaskFunc(funcMap);

    createProcPool(pool, funcMap);

    // 注册信号处理函数，捕获Ctrl+C信号(SIGINT)
    signal(SIGINT, signalHandler);

    int taskNum = funcMap.size();
    // 使用 C++11 中的随机数生成器替代 rand()
    std::default_random_engine generator(std::time(0));
    std::uniform_int_distribution<int> distribution(0, funcMap.size());

    while (true)
    {
        int subIndex = distribution(generator);
        int taskIndex = distribution(generator);

        sendTask(pool[subIndex], taskIndex);
        sleep(3);
    }

    cleanupResources(pool);
    waitChildProcesses(pool);

    return 0;
}
