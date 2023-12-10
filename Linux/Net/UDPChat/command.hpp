#pragma once

#include "udpServer.hpp"
#include <memory>
#include <unordered_map>
#include <fstream>
#include <signal.h>
#include <cstdio>

/* 远端命令行 */
void execCommand(std::string serverIp, uint16_t serverPort, std::string cmd, int sockfd)
{
    bool flag = true;

    /* 去除不合法指令 */
    if (cmd.find("rm") != std::string::npos || cmd.find("rmdir") != std::string::npos || cmd.find("mv") != std::string::npos)
        flag = false;

    std::string responseMessage;
    if (flag)
    {
        /* cmd解析 */
        FILE *fp = popen(cmd.c_str(), "r");
        if (fp == nullptr)
            responseMessage = cmd + " exec fail";

        char lineBuffer[1024];
        while (fgets(lineBuffer, sizeof(lineBuffer) - 1, fp))
            responseMessage += lineBuffer;

        if (responseMessage.empty())
            responseMessage += "\n";

        pclose(fp);
    }
    else
        responseMessage = "illegal command\n";

    /* 返回结果 */
    struct sockaddr_in client;
    bzero(&client, sizeof(client));
    /* 填充客户端信息 */
    client.sin_family = AF_INET;
    client.sin_port = htons(serverPort);
    client.sin_addr.s_addr = inet_addr(serverIp.c_str());
    socklen_t clientLen = sizeof(client);

    sendto(sockfd, responseMessage.c_str(), responseMessage.size(),
           0, (struct sockaddr *)&client, clientLen);
}