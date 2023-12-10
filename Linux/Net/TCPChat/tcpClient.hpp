#pragma once

#include <iostream>
#include <string>

class TcpClient
{
public:
    /* 客户端构造函数 */
    TcpClient(const std::string &serverIp, const uint16_t &serverPort)
        : _sock(-1), _serverIp(serverIp), _serverPort(serverPort) {}

    /* 客户端初始化 */
    void init()
    {
        
    }

    void start() {}

    ~TcpClient() {}

private:
    int _sock;             /* 套接字 */
    std::string _serverIp; /* 服务器IP */
    uint16_t _serverPort;  /* 服务器端口 */
};