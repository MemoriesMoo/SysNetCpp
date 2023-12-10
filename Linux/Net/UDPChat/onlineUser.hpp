#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class User
{
public:
    User(const std::string &ip, const uint16_t &port) : _ip(ip), _port(port) {}
    ~User() {}
    std::string ip() { return _ip; }
    uint16_t port() { return _port; }

private:
    std::string _ip;
    uint16_t _port;
};

class OnlineUser
{
public:
    OnlineUser() {}
    ~OnlineUser() {}
    void add(const std::string &ip, const uint16_t &port)
    {
        /* 根据ip+port构建用户信息 */
        std::string id = ip + "-" + std::to_string(port);
        users.insert(make_pair(id, User(ip, port)));
    }

    void del(const std::string &ip, const uint16_t &port)
    {
        /* 根据信息删除在线用户 */
        std::string id = ip + "-" + std::to_string(port);
        users.erase(id);
    }

    bool isOnline(const std::string &ip, const uint16_t &port)
    {
        /* 根据信息判断是否在线决定是否添加 */
        std::string id = ip + "-" + std::to_string(port);
        return users.find(id) == users.end() ? false : true;
    }

    void broadcastMessage(int sockfd, const std::string &message, const std::string &ip, const uint16_t &port)
    {
        for (auto &user : users)
        {
            /* 构建客户端信息 */
            struct sockaddr_in client;
            bzero(&client, sizeof(client));

            client.sin_family = AF_INET;
            client.sin_port = htons(user.second.port());
            client.sin_addr.s_addr = inet_addr(user.second.ip().c_str());
            /* 类似用户名信息 可以内置在User类中 */
            std::string responseMessage = ip + "-" + std::to_string(port) + "# ";
            responseMessage += message;
            sendto(sockfd, responseMessage.c_str(), responseMessage.size(), 0, (struct sockaddr *)&client, sizeof(client));
        }
    }

private:
    std::unordered_map<std::string, User> users;
};