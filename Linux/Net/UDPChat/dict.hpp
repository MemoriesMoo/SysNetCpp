#pragma once

#include "udpServer.hpp"
#include <memory>
#include <unordered_map>
#include <fstream>
#include <signal.h>

/* 存储字典容器 */
std::unordered_map<std::string, std::string> dict;
/* 字典数据文件 */
const std::string dictFile = "./dict.txt";

/* 切割字符串函数 用于加载字典 */
static bool cutString(const std::string &target, const std::string &sep,
                      std::string *key, std::string *value)
{
    /* 获取分隔符sep起始索引 */
    auto pos = target.find(sep);
    /* 格式不符合 */
    if (pos == std::string::npos)
        return false;

    /* 切割target填充key和value */
    *key = target.substr(0, pos);
    *value = target.substr(pos + sep.size());
    return true;
}

/* 加载字典 */
static void initDict()
{
    std::ifstream in(dictFile, std::ios::binary);
    if (!in.is_open())
    {
        std::cerr << "open error:" << errno
                  << " " << strerror(errno) << std::endl;
        exit(OPEN_ERR);
    }

    std::string line;
    std::string key, value;
    while (getline(in, line))
    {
        if (cutString(line, ":", &key, &value))
            dict.insert(std::make_pair(key, value));
        else
        {
            std::cerr << "file format error:" << line << std::endl;
            exit(FILE_FORMAT_ERR);
        }
    }

    in.close();

    /* 日志 */
    std::cout << "load dict sucess" << std::endl;
}

/* 用于输出检测字典是否加载成功 */
static void debugPrint()
{
    for (auto &dt : dict)
    {
        std::cout << dt.first << " # "
                  << dt.second << std::endl;
    }
}

/* 业务处理 */
static void handlerMessage(std::string serverIp,
                           uint16_t serverPort, std::string message,
                           int sockfd)
{
    /* 业务处理 不关心message来源 */
    /* 实现通信与业务解耦 */

    std::string responseMessage;
    auto res = dict.find(message);

    if (res == dict.end())
        /* 未知单词 */
        responseMessage = "Unknow";
    else
        responseMessage = res->second;

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

/* 单词列表热加载 */
void reload(int signo)
{
    (void)signo;
    /* 重新加载字典 */
    initDict();
}