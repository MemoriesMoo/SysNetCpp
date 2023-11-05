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

namespace Client
{
    enum
    {
        SOCKET_ERR = 2,
        SENT_ERR = 3
    };

    class udpClient
    {
    public:
        // 构造函数，初始化服务器IP地址和端口号
        udpClient(const std::string &serverIp, const uint16_t &serverPort)
            : _serverIp(serverIp),
              _serverPort(serverPort),
              _sockfd(-1)
        {
        }

        // 初始化客户端，创建套接字
        void initClient()
        {
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);

            // 检查套接字创建是否成功
            if (_sockfd == -1)
            {
                std::cerr << "socket error " << errno << ": " << strerror(errno) << std::endl;
                exit(SOCKET_ERR);
            }
        }

        // 发送数据到服务器
        void sendData(const std::string &data)
        {
            struct sockaddr_in serverAddr;
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(_serverPort);
            serverAddr.sin_addr.s_addr = inet_addr(_serverIp.c_str());

            ssize_t sentBytes = sendto(_sockfd, data.c_str(), data.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

            if (sentBytes == -1)
            {
                std::cerr << "sendto error " << errno << ": " << strerror(errno) << std::endl;
                exit(SENT_ERR);
            }
        }

        // 析构函数，关闭套接字
        ~udpClient()
        {
            close(_sockfd);
        }

    private:
        std::string _serverIp; // 服务器IP地址
        uint16_t _serverPort;  // 服务器端口号
        int _sockfd;           // 套接字文件描述符
    };
}
