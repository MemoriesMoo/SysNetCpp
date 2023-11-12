#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <functional>
#include <ctime>
#include <random>
#include <csignal>
#include <sys/wait.h>

#define POOL_NUM 5

typedef void (*func_t)();

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

// 将任务函数添加到任务函数向量中
void loadTaskFunc(std::vector<func_t> &funcMap)
{
    funcMap.push_back(downloadTask);
    funcMap.push_back(ioTask);
    funcMap.push_back(flushTask);
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

// 创建进程池
void createProcPool(std::vector<SubProc> &pool, std::vector<func_t> &funcMap)
{
    for (int cnt = 0; cnt < POOL_NUM; ++cnt)
    {
        int fds[2]{0};
        int res = pipe(fds);
        if (res != 0)
        {
            std::cerr << "创建管道失败\n";
            exit(EXIT_FAILURE);
        }

        pid_t id = fork();
        if (id == 0)
        {
            close(fds[1]); // 关闭用于写入的文件描述符

            while (true)
            {
                int commandCode = recTask(fds[0]);
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

        close(fds[0]); // 关闭用于读取的文件描述符

        SubProc sub(id, fds[1]);
        pool.push_back(std::move(sub));
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
    cleanupResources(pool);
    exit(signum);
}

// 全局变量，用于保存进程池
std::vector<SubProc> pool;

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
