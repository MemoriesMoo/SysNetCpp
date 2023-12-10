#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
        local.sin_addr.s_addr = INADDR_ANY; /* 默认任意IP */
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
            /* version1.0 */
            serviceIO(sockFd);

            /* 运行到此处代表此fd功能已经完成 需要关闭*/
            /* 否则会导致文件描述符泄露 */
            close(sockFd);
        }
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
                outBuffer += "server echo";

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