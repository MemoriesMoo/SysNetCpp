#include "tcpClient.hpp"
#include <memory>

/* 使用说明 */
static void usage(std::string proc)
{
    std::cout << "\nUsage:\n\t"
              << proc << " server_ip server_port \n\n";

    logMessage(FATAL, "Tcpclient use illegal.");
}

/*  使用方法 */
/* ./tcpClient server_ip server_port */
int main(int argc, char *argv[])
{
    /* 若命令行程序启动方式不正确 */
    if (argc != 3)
    {
        usage(argv[0]);
        exit(USAGE_ERROR);
    }
    logMessage(NORMAL, "Tcpclient start success.");

    /* 命令行获取服务器IP与port信息 */
    std::string serverIp = argv[1];
    uint16_t serverPort = atoi(argv[2]);

    /* 构建客户端对象 */
    std::unique_ptr<TcpClient> tcli(new TcpClient(serverIp, serverPort));

    /* 初始化并启动客户端 */
    tcli->init();
    tcli->start();

    return 0;
}