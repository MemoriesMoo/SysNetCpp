#pragma once

#include "udpServer.hpp"
#include "onlineUser.hpp"
#include <memory>
#include <unordered_map>

/* 定义全局onlineUser对象 */
/* 内含一个空unordered_map */
OnlineUser onlineUser;

/* 路由转发消息 */
void routeMessage(std::string ip, uint16_t port, std::string message, int sockfd)
{
    /* 登录 */
    if (message == "online\n")
        onlineUser.add(ip, port);

    /* 退出 */
    if (message == "offonline\n")
        onlineUser.del(ip, port);

    /* 判断用户是否在线 */
    if (onlineUser.isOnline(ip, port))
        /* 在线则对消息进行路由 */
        onlineUser.broadcastMessage(sockfd, message, ip, port);
    else
    {
        /* 不在线则提示登陆 */
        /* 返回结果 */
        struct sockaddr_in client;
        bzero(&client, sizeof(client));
        /* 填充客户端信息 */
        client.sin_family = AF_INET;
        client.sin_port = htons(port);
        client.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t clientLen = sizeof(client);

        std::string responseMessage = "请先登录 输入online即可\n";
        sendto(sockfd, responseMessage.c_str(), responseMessage.size(), 0, (struct sockaddr *)&client, clientLen);
    }
}
