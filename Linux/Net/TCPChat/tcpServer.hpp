#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "log.hpp" /* 日志系统头文件 */

/* 自定义退出错误码 */
enum
{
    USAGE_ERROR = 1, /* 程序启动方式有误 */
    SOCKET_ERROR,    /* 创建套接字失败 */
    BIND_ERROR,      /* bind绑定信息失败 */
    LISTEN_ERROR     /* listen监听失败 */
};

static uint16_t globalPort = 8080; /* 服务器默认端口号 */
static const int gBackLog = 5;
class TcpServer;

/* 线程函数参数信息类 */
class ThreadData
{
public:
    /* 参数对象构造函数 */
    ThreadData(TcpServer *self, int sock)
        : _self(self), _sock(sock) {}

public:
    TcpServer *_self; /* TcpServer对象this指针 */
    int _sock;        /* 线程通信套接字*/
};

class TcpServer
{
public:
    /* 服务器构造函数 */
    TcpServer(const uint16_t &port = globalPort)
        : _listenSock(-1), _port(port) {}

    /* 初始化TCP服务器 */
    void init()
    {
        /* 1.创建socket文件套接字对象 */
        _listenSock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listenSock == -1)
        {
            /* 创建套接字失败日志信息 */
            logMessage(FATAL, "Create socket error.");
            exit(SOCKET_ERROR);
        }
        logMessage(NORMAL, "Create socket success.");

        /* 2.bind绑定网络信息 */
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));

        /* 填充相关信息 */
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);      /* 主机转网络序列 */
        local.sin_addr.s_addr = INADDR_ANY; /* 默认任意IP 0.0.0.0 */
        socklen_t loaclLen = sizeof(local);

        /* bind绑定 */
        int ret = bind(_listenSock, (struct sockaddr *)&local, loaclLen);
        if (ret == -1)
        {
            /* 绑定失败日志 */
            logMessage(FATAL, "Bind socket error.");
            exit(BIND_ERROR);
        }
        logMessage(NORMAL, "Bind socket success.");

        /* 3.设置socket监听状态 */
        /* 第二个参数暂时不深入理解 */
        ret = listen(_listenSock, gBackLog);
        if (ret == -1)
        {
            /* 监听失败日志 */
            logMessage(FATAL, "Listen socket error.");
            exit(LISTEN_ERROR);
        }
        logMessage(NORMAL, "Listen socket success.");
    }

    void start()
    {
        /* 多进程解决方式二 较为合适 */
        /* 父进程不再关心子进程退出状态 由操作系统自动处理 */
        // signal(SIGCHLD, SIG_IGN);

        /* 4.获取新链接 */
        while (true)
        {
            /* 此处获取的sock用于与client通信的fd */
            struct sockaddr_in peer;
            socklen_t peerLen = sizeof(peer);
            int sockFd = accept(_listenSock, (struct sockaddr *)&peer, &peerLen);
            if (sockFd == -1)
            {
                /* 链接失败输出日志 继续获取链接 */
                logMessage(ERROR, "Accept error.");
                continue;
            }

            /* 获取新链接成功 */
            logMessage(NORMAL, "Accept new link success");
            std::cout << "sockFd: " << sockFd << std::endl;

            /* 5.使用获取的sockFd通信 */
            /* 面向字节流通信 与文件操作相同 */

            /* version1.0  单进程版本通信 */
            // serviceIO(sockFd);
            /* 运行到此处代表此fd功能已经完成 需要关闭*/
            /* 否则会导致文件描述符泄露 */
            // close(sockFd);

            /* version2.0 多进程版本 */
            // pid_t id = fork();
            // if (id == 0)
            // {
            //     /* 子进程继承文件描述符表 最好关闭不需要的文件描述符 */
            //     /* 子进程提供通信服务 */
            //     close(_listenSock);

            //     /* 多进程解决方式一 */
            //     /* 子进程中进行fork 并直接关闭子进程 留下孙子进程 */
            //     /* 子进程退出 孙子进程被操作系统领养 */
            //     // if (fork() > 0)
            //     //     exit(0);

            //     /* 利用孙子进程进行通信 */
            //     serviceIO(sockFd);
            //     close(sockFd);
            //     exit(0);
            // }

            /* 父进程也需要关闭该sockFd 否则fd会一直增大 造成泄露 */
            // close(sockFd);
            /* 父进程部分-创建子进程之后需要等待子进程 */
            /* 如果此处需要等待子进程 和单进程版本没有很大区别 解决方法如上所述 利用孙子进程 */
            /* 子进程退出的话父进程直接等待成功 接着继续获取新链接 */
            // pid_t ret = waitpid(id, nullptr, 0);
            // if (ret > 0)
            //     std::cout << "Wait success " << ret << std::endl;

            /* version3.0  多线程版本 */
            /* 创建线程 */
            pthread_t tid;
            ThreadData *td = new ThreadData(this, sockFd);
            pthread_create(&tid, nullptr, threadRoutine, td);

            /* 若要等待线程 依然与单进程无异 */
            /* 线程也没有非阻塞等待 */
            /* 解决方法：线程分离 */
            // pthread_join(tid, nullptr);
        }
    }

    /* 为什么是static？*/
    /* 因为线程调用函数只能由这一个参数 */
    /* 如果不是static函数则会有隐藏的this指针 不符合要求 */
    static void *threadRoutine(void *args)
    {
        /* 分离线程 使得主线程可以继续建立新链接 无需关心此线程 */
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData *>(args);

        /* 调用对应的通信代码 */
        td->_self->serviceIO(td->_sock);
        delete td;
        /* 文件描述符共享 使用完得关闭 */
        close(td->_sock);
        return nullptr;
    }

    void serviceIO(int sock)
    {

        while (true)
        {
            /* 消息缓冲区 */
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));

            ssize_t readBytes = read(sock, buffer, sizeof(buffer) - 1);
            if (readBytes > 0)
            {
                /* 暂时把读取到的数据当作字符串处理 */
                std::cout << "recv message: " << buffer << std::endl;

                std::string outBuffer = buffer;
                outBuffer += "[server echo]";

                write(sock, outBuffer.c_str(), outBuffer.size());
            }
            else if (readBytes == 0)
            {
                /* 代表客户端退出 */
                logMessage(NORMAL, "Client quit, server quit.");
                break;
            }
        }
    }

    ~TcpServer() {}

private:
    int _listenSock; /* 套接字 用于监听链接、获取链接*/
    uint16_t _port;  /* 服务器端口号 */
};