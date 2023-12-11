#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.hpp"

/* 自定义退出错误码 */
enum
{
    USAGE_ERROR = 1, /* 程序启动方式有误 */
    SOCKET_ERROR,    /* 创建套接字失败 */
    CONN_ERROR       /* 链接服务器失败 */
};

class TcpClient
{
public:
    /* 客户端构造函数 */
    TcpClient(const std::string &serverIp, const uint16_t &serverPort)
        : _sock(-1), _serverIp(serverIp), _serverPort(serverPort) {}

    /* 客户端初始化 */
    void init()
    {
        /* 1.创建套接字对象 */
        _sock = socket(AF_INET, SOCK_STREAM, 0);
        if (_sock == -1)
        {
            /* 创建套接字失败日志信息 */
            logMessage(FATAL, "Create socket error.");
            exit(SOCKET_ERROR);
        }
        logMessage(NORMAL, "Create socket success.");

        /* 2.Tcp客户端需要显示bind绑定呢？*/
        /* 不需要显示bind port由操作系统自动随机绑定 */
        /* 机器上很多网络程序运行 如果显示绑定端口会造成冲突 */

        /* 3.Tcp客户端是否需要listen？*/
        /* 不需要 服务器才需要具备listen功能 */
        /* 因为服务器等待客户端去链接 而客户端市区链接服务器 无需监听功能 */

        /* 4.Tcp客户端需要accept吗？*/
        /* 不需要 原理解析同listen功能 */
        /* 客户端是去链接服务器 不监听当然也就不需要accept */
    }

    void start()
    {
        /* 5.客户端要发起connect链接请求 链接成功才能通信 */
        /* 此过程也会自动bind绑定网络信息 */

        /* 创建服务器结构对象填充对应信息 */
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));

        server.sin_family = AF_INET;
        server.sin_port = htons(_serverPort);
        server.sin_addr.s_addr = inet_addr(_serverIp.c_str());
        socklen_t serverLen = sizeof(server);

        int ret = connect(_sock, (struct sockaddr *)&server, serverLen);
        if (ret == -1)
        {
            /* 链接建立失败 */
            logMessage(FATAL, "Socket connect error");
            exit(CONN_ERROR);
        }
        logMessage(NORMAL, "Socket connect success");

        /* 链接成功之后进行通信 */
        while (true)
        {
            std::string message;

            std::cout << "enter# ";
            std::getline(std::cin, message);

            /* 发送消息 */
            write(_sock, message.c_str(), message.size());

            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));

            int readBytes = read(_sock, buffer, sizeof(buffer) - 1);
            if (readBytes > 0)
            {
                /* 暂时把读取到的数据当作字符串处理 */
                std::cout << "recv message: " << buffer << std::endl;
            }
            else if(readBytes == 0)
            {
                logMessage(FATAL, "server error, quit.");
                break;
            }
        }
    }

    ~TcpClient() {}

private:
    int _sock;             /* 套接字 */
    std::string _serverIp; /* 服务器IP */
    uint16_t _serverPort;  /* 服务器端口 */
};