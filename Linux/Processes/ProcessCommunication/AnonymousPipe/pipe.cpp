/* 功能概述：匿名管道（内存级文件）用于父子进程间通信 */

#include <iostream>
#include <unistd.h>
#include <cassert>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <ctime>

/*
    读快写慢？
    读慢写快？
    写关闭，读到0？
    读关闭，写？
*/

// 获取当前时间并以字符串形式返回
std::string getCurrentTime()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-d %X", &tstruct);
    return buf;
}

// 打印带有指定颜色的消息
void printColoredMessage(const std::string &message, const std::string &color)
{
    std::cout << "\033[" << color << "m" << message << "\033[0m" << std::endl;
}

int main()
{
    int fds[2];

    // 创建一个管道
    int pipe_res = pipe(fds);
    assert(pipe_res == 0);

    pid_t id = fork();
    assert(id >= 0);

    if (id == 0)
    {
        // 在子进程中

        // 关闭管道的读端
        close(fds[0]);

        // 子进程通信代码
        const char *str = "I'm the child process";
        int cnt = 0;
        while (true)
        {
            char buffer[1024];
            // 从子进程向父进程发送带有时间戳和自定义事件内容的事件通知
            snprintf(buffer, sizeof(buffer), "Child[%d]: Event - Type: Alert, Description: Security breach detected", cnt++);
            std::string message = getCurrentTime() + " | Type: Event | Sender: Child | " + buffer;
            write(fds[1], message.c_str(), message.length());
            // 每秒写入一次
            sleep(4);
        }

        // 关闭管道的写端并退出子进程
        close(fds[1]);
        exit(0);
    }

    // 在父进程中

    // 关闭管道的写端
    close(fds[1]);

    // 父进程通信代码
    int cnt = 0;
    while (true)
    {
        char buffer[1024];
        // 从子进程接收事件通知
        ssize_t s = read(fds[0], buffer, sizeof(buffer) - 1);

        if (s > 0)
        {
            buffer[s] = '\0';
            // 打印带有时间戳、彩色输出和发送方信息的事件通知
            std::string receivedMessage = std::string(buffer);
            std::string timestamp = getCurrentTime();
            std::string senderInfo = receivedMessage.substr(receivedMessage.find("Sender:") + 8);
            std::string eventType = receivedMessage.substr(receivedMessage.find("Type:") + 6, 9);
            std::string eventDescription = receivedMessage.substr(receivedMessage.find("Description:") + 13);
            std::string coloredMessage = "Parent[" + std::to_string(cnt++) + "]: " + buffer + " (" + timestamp + ")";
            printColoredMessage(coloredMessage, "32"); // 32表示绿色
            std::cout << "   Event Type: " << eventType << std::endl;
            std::cout << "   Event Description: " << eventDescription << std::endl;
            std::cout << "   Sender: " << senderInfo << std::endl;
        }
    }

    // 等待子进程完成
    pid_t child_status = waitpid(id, nullptr, 0);
    assert(child_status == id);

    // 关闭管道的读端并退出父进程
    close(fds[0]);
    return 0;
}