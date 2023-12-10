#include "tcpServer.hpp"
#include <memory>

/* 使用说明 */
static void usage(std::string proc)
{
    std::cout << "\nUsage:\n\t"
              << proc << " local_port\n\n";

    logMessage(FATAL, "Usage illegal.");
}

/* 启动方法 类似udpServer */
/* ./tcpServer local_port */
int main(int argc, char *argv[])
{
    /* 若命令行程序启动方式不正确 */
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERROR);
    }
    logMessage(NORMAL, "Proc start success.");

    /* 获取使用的端口号 */
    uint16_t localPort = atoi(argv[1]);

    /* 创建服务器对象并启动 */
    std::unique_ptr<TcpServer> tsvr(new TcpServer());
    tsvr->init();
    tsvr->start();

    return 0;
}