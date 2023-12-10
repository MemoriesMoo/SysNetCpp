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
#include <pthread.h>
#include <mutex>

enum
{
    SOCKET_ERR = 2,
    BIND_ERR = 3,
    USAGE_ERR = 4
};

class udpClient
{
public:
    udpClient(const std::string &serverIp, const uint16_t &serverPort)
        : _serverIp(serverIp), _serverPort(serverPort), _sockfd(-1) {}

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

        /* Client是否需要绑定？ 必须要 */
        /* Client是否需要明确显示绑定？ 不需要 */
        /* OS自动分配端口进行bind绑定 */
        /* 为什么server需要显示绑定呢？ */
        /* Server需要明确的端口号，因此需要显示绑定 */
        /* 客户端也需要端口号，但并不是很重要 */
    }

    static void *readMessage(void *args)
    {
        /* 分离线程 */
        pthread_detach(pthread_self());
        /* 获取sockfd */
        int sockfd = *(static_cast<int *>(args));
        while (true)
        {
            /* 接收服务端发回的结果 */
            char responseMessage[1024]{0};
            struct sockaddr_in temp;
            socklen_t tempLen = sizeof(temp);
            recvfrom(sockfd, responseMessage, sizeof(responseMessage) - 1,
                     0, (struct sockaddr *)&temp, &tempLen);
            // std::cout << "Server response: " << responseMessage;
            std::cout << responseMessage;
        }

        return nullptr;
    }

    void run()
    {
        pthread_create(&_reader, nullptr, readMessage, (void *)&_sockfd);

        /* 要发送的信息 */
        std::string message;
        /* 信息缓冲区 */
        char buffer[1024];

        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        /* 填充服务端结构体信息 */
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(_serverIp.c_str());
        server.sin_port = htons(_serverPort);
        socklen_t serverLen = sizeof(server);

        while (true)
        {
            std::cout << "Please enter message: ";
            // std::cin >> message;
            /* 获取行输入 */
            fgets(buffer, sizeof(buffer) - 1, stdin);
            message = buffer;

            ssize_t sendBytes = sendto(_sockfd, message.c_str(), message.length(),
                                       0, (struct sockaddr *)&server, serverLen);
        }
    }

    ~udpClient() {}

private:
    std::string _serverIp; /* 服务端IP */
    uint16_t _serverPort;  /* 服务端端口 */
    int _sockfd;

    pthread_t _reader; /* 读取消息线程 */
    pthread_t _writer; /* 发送消息线程*/
};