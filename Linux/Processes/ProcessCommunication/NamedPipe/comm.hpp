#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <cassert>

// 定义命名管道的路径
#define NAMED_PIPE "./myPipe"

// 创建命名管道的函数
bool createFifo(const std::string &path)
{
    // 设置文件权限掩码为0，确保后续创建的文件权限不受限制
    umask(0);

    // 使用 mkfifo 函数创建命名管道，权限为 0600
    if (mkfifo(path.c_str(), 0600) == 0)
        return true;
    else
        throw std::runtime_error("无法创建命名管道: " + std::string(strerror(errno)));
}

// 移除命名管道的函数
void removeFifo(const std::string &path)
{
    // 使用 unlink 函数删除命名管道
    if (unlink(path.c_str()) != 0)
        throw std::runtime_error("无法删除命名管道: " + std::string(strerror(errno)));
}

