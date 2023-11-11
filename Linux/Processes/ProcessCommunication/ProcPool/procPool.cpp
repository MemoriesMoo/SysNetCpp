#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <cassert>
#include <cstring>

#define PROC_NUM 5

// 设计类存储子进程相关信息
class subProc
{
public:
    // 带参构造函数
    subProc(std::string name, pid_t subId, int writeFd)
        : name_(name),
          subId_(subId),
          writeFd_(writeFd) {}

    std::string name_; // 进程标识
    pid_t subId_;      // 进程ID
    int writeFd_;      // 管道写端
};

void createSubProc(std::vector<subProc> &subs)
{
    // 创建不同子进程
    for (int cnt = 0; cnt < PROC_NUM; ++cnt)
    {
        // 存储读写段fd
        int fds[2]{0};

        // 为子进程与父进程创建匿名管道
        int num = pipe(fds);
        // 创建成功返回0
        assert(num == 0);

        // 创建子进程
        pid_t id = fork();
        // 创建成功返回非负整数
        assert(id >= 0);

        // 子进程读取数据处理
        if (id == 0)
        {
            // 关闭写段
            close(fds[1]);

            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            ssize_t s = read(fds[0], buffer, sizeof(buffer) - 1);

            if (s == 0)
            {
                exit(0);
            }

            // 读取到数据
            std::cout << buffer << std::endl;

            // 程序结束
            exit(0);
        }

        std::string name("subProc" + cnt);

        // 创建子进程subProc对象
        subProc sub(name, id, fds[1]);

        subs.push_back(sub);

        // 父进程关闭读端
        close(fds[0]);
    }
}

int main()
{
    // 创建vector存储子进程subProc对象
    std::vector<subProc> subs;

    // 构造函数创建子进程
    createSubProc(subs);
    
    // 父进程分别给子进程发送数据
    for (int cnt = 0; cnt < PROC_NUM; cnt++)
    {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%s:%d", "My pid is ", subs[cnt].subId_);
        write(subs[cnt].writeFd_, buffer, strlen(buffer));
        sleep(2);
    }
}