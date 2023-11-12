#include "comm.hpp"

int main()
{
    // 创建命名管道
    bool res = createFifo(NAMED_PIPE);
    assert(res);
    (void)res;

    std::cout << "服务器启动！等待客户端启动中...\n";

    // 以只读方式打开命名管道
    int rfd = open(NAMED_PIPE, O_RDONLY);
    std::cout << "客户端已启动，可以开始通信！\n";

    // 检查文件描述符是否有效
    if (rfd < 0)
        exit(1);

    while (true)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // 从命名管道中读取数据
        ssize_t readBytes = read(rfd, buffer, sizeof(buffer) - 1);

        if (readBytes > 0)
        {
            // 显示从客户端接收到的数据
            std::cout << "客户端 -> 服务器: " << buffer << "\n";
        }
        else if (readBytes == 0)
        {
            // 客户端已关闭连接，退出循环
            std::cout << "客户端退出，服务器也退出!\n";
            break;
        }
        else
        {
            // 处理读取错误
            std::cout << "错误: errno " << errno << " | 错误信息: " << strerror(errno) << std::endl;
            break;
        }
    }

    // 关闭文件描述符
    close(rfd);

    // 移除命名管道
    removeFifo(NAMED_PIPE);
}
