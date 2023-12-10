#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>
#include <strings.h>
#include <unistd.h>
#include <functional>

typedef std::function<void(std::string, uint16_t, std::string, int)> func_t;

enum
{
    SOCKET_ERR = 2,
    BIND_ERR,
    USAGE_ERR,
    OPEN_ERR,
    FILE_FORMAT_ERR
};

class udpServer
{
    static const std::string defaultIp;

public:
    udpServer(const func_t &callBack, const uint16_t &port, const std::string &ip = defaultIp)
        : _callBack(callBack), _port(port), _ip(ip), _sockfd(-1) {}

    void init()
    {
        /* 1.创建套接字 */
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd == -1)
        {
            std::cerr << "Socket error: " << errno
                      << " (" << strerror(errno) << ")" << std::endl;
            exit(SOCKET_ERR);
        }

        /* 2.绑定 port 和 ip */
        struct sockaddr_in local;
        bzero(&local, sizeof(local));

        /* 协议 */
        local.sin_family = AF_INET;
        /* 通信双方都要能获取对方的 IP 与端口号 */
        /* 多字节涉及网络中的大小端问题 */
        local.sin_port = htons(_port);
        /* string -> uint32 + htonl() == inet_addr() */
        /* C 语言结构体可以直接赋值吗？ 不行，所以得理清内部结构 */
        // local.sin_addr.s_addr = inet_addr(_ip.c_str());
        local.sin_addr.s_addr = htonl(INADDR_ANY); /* 任意地址访问 */

        int ret = bind(_sockfd, (sockaddr *)&local, sizeof(local));
        if (ret == -1)
        {
            std::cerr << "Bind error: " << errno
                      << " (" << strerror(errno) << ")" << std::endl;
            exit(BIND_ERR);
        }
    }

    void start()
    {
        /* 服务器的本质：无限循环，常驻内存 注意持续内存泄露问题 */
        /* 读取数据 */
        while (true)
        {
            char messBuffer[1024]{0};
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            ssize_t readBytes = recvfrom(_sockfd, messBuffer, sizeof(messBuffer) - 1, 0, (sockaddr *)&client, &len);

            /* 收到的数据是谁发送的？*/
            if (readBytes > 0)
            {
                /* ip -> 点分十进制  inet_ntoa() */
                std::string clientIp = inet_ntoa(client.sin_addr);
                /* 将网络中的端口号进行转化 */
                uint16_t clientPort = ntohs(client.sin_port);
                std::string message = messBuffer;

                /* 展示收到的消息信息 */
                std::cout << "Received from [" << clientIp << "] "
                          << "at port [" << clientPort << "]: "
                          << message;

                /* 如何对数据做处理？*/
                _callBack(clientIp, clientPort, message, _sockfd);
            }
        }
    }

    ~udpServer()
    {
    }

private:
    uint16_t _port;  /* 服务器端口号 */
    std::string _ip; /* 服务器 IP 地址 */
    int _sockfd;
    func_t _callBack; /* 回调函数 */
};

/* 默认 IP 地址 点分十进制风格 */
const std::string udpServer::defaultIp = "0.0.0.0";