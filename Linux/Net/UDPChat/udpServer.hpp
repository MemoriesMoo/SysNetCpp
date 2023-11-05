#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>

namespace Server
{
    const static std::string defaultIp = "0.0.0.0";
    enum
    {
        SOCKET_ERR = 2,
        BIND_ERR = 3
    };

    class udpServer
    {
    public:
        // 构造函数，初始化端口号和IP地址
        udpServer(const uint16_t &port, const std::string &ip = defaultIp)
            : _port(port),
              _ip(ip),
              _sockfd(-1)
        {
        }

        // 初始化服务器
        void initServer()
        {
            // 创建套接字
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);

            // 检查套接字创建是否成功
            if (_sockfd == -1)
            {
                std::cerr << "socket error " << errno << ": " << strerror(errno) << std::endl;
                exit(SOCKET_ERR);
            }

            // 准备本地套接字地址
            struct sockaddr_in local;
            local.sin_family = AF_INET;                     // 地址族为IPv4
            local.sin_port = htons(_port);                  // 设置端口号，需要使用htons函数进行字节序转换
            local.sin_addr.s_addr = inet_addr(_ip.c_str()); // 设置IP地址

            // 绑定套接字到本地地址
            int res = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));

            // 检查是否绑定成功
            if (res == -1)
            {
                std::cerr << "bind error " << errno << ": " << strerror(errno) << std::endl;
                exit(BIND_ERR);
            }
        }

        // 启动服务器，接收和处理UDP数据包
        void start()
        {
            while (true)
            {
                char buffer[1024]; // 用于接收数据的缓冲区
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);

                ssize_t bytesRead = recvfrom(_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &clientAddrLen);

                if (bytesRead > 0)
                {
                    // 处理接收到的数据
                    processReceivedData(buffer, bytesRead, clientAddr);
                }
            }
        }

        // 处理接收到的UDP数据包
        void processReceivedData(const char *data, ssize_t dataSize, const struct sockaddr_in &clientAddr)
        {
            // data: 接收到的数据
            // dataSize: 数据的大小
            // clientAddr: 发送数据的客户端地址信息

            // 将接收到的数据打印到标准输出
            std::cout << "Received " << dataSize << " bytes from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
            std::cout << "Data: " << std::string(data, dataSize) << std::endl;
        }

        // 析构函数，关闭套接字
        ~udpServer()
        {
            close(_sockfd); // 关闭套接字
        }

    private:
        uint16_t _port;  // 服务器端口号
        std::string _ip; // 服务器IP地址
        int _sockfd;     // 套接字文件描述符
    };
}
