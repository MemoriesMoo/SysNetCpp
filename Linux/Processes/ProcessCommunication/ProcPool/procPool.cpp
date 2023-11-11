#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <array>
#include <cassert>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>

#define POOL_SIZE 5

// 子进程信息的类
class SubProc
{
public:
    // 构造函数，初始化子进程信息
    SubProc(std::string name, pid_t subId, int writeFd, int readFd)
        : name_(name),
          subId_(subId),
          writeFd_(writeFd),
          readFd_(readFd) {}

    std::string name_; // 进程标识
    pid_t subId_;      // 进程ID
    int writeFd_;      // 管道写端
    int readFd_;       // 管道读端
};

// 创建进程池的函数
void createProcessPool(std::vector<SubProc> &pool)
{
    for (int cnt = 0; cnt < POOL_SIZE; ++cnt)
    {
        // 用于父子进程通信的管道
        std::array<int, 2> writePipe{0, 0};
        std::array<int, 2> readPipe{0, 0};

        // 创建管道，检查是否成功
        int writePipeResult = pipe(writePipe.data());
        int readPipeResult = pipe(readPipe.data());

        assert(writePipeResult == 0 && readPipeResult == 0);

        // 创建子进程
        pid_t id = fork();
        assert(id >= 0);

        if (id == 0)
        {
            // 子进程关闭写端，只关心读端
            close(writePipe[1]);
            close(readPipe[0]);

            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));

            // 从管道读取数据
            ssize_t readSize = read(writePipe[0], buffer, sizeof(buffer) - 1);

            if (readSize == 0)
            {
                // 如果没有读取到数据，子进程退出
                exit(0);
            }

            // 打印接收到的数据
            std::cout << "Child process " << getpid() << " receive message: " << buffer << std::endl;

            // 子进程可以在这里执行具体的任务

            // 子进程结束前关闭读端
            close(writePipe[0]);
            close(readPipe[1]);

            // 子进程结束
            exit(0);
        }

        // 父进程关闭读端，只关心写端
        close(writePipe[0]);
        close(readPipe[1]);

        // 为每个子进程创建唯一名称
        std::string name = "SubProc" + std::to_string(cnt);

        // 创建 SubProc 对象并存储在向量中
        SubProc sub(name, id, writePipe[1], readPipe[0]);
        pool.push_back(sub);
    }
}

int main()
{
    // 创建存储子进程信息的向量
    std::vector<SubProc> processPool;
    // 使用函数创建进程池
    createProcessPool(processPool);

    // 向每个子进程发送数据
    for (int cnt = 0; cnt < POOL_SIZE; cnt++)
    {
        char buffer[1024];
        // 格式化要发送的消息
        snprintf(buffer, sizeof(buffer), "Parent process send message to child process, child process's pid:%d", processPool[cnt].subId_);

        // 将消息写入管道，传递给子进程
        write(processPool[cnt].writeFd_, buffer, strlen(buffer));

        // 等待一段时间，以便观察子进程输出
        sleep(2);
    }

    // 等待所有子进程结束
    for (int cnt = 0; cnt < POOL_SIZE; cnt++)
    {
        waitpid(processPool[cnt].subId_, nullptr, 0);
    }

    // 关闭所有管道
    for (int cnt = 0; cnt < POOL_SIZE; cnt++)
    {
        close(processPool[cnt].writeFd_);
        close(processPool[cnt].readFd_);
    }

    return 0;
}
