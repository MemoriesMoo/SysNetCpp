#include "udpServer.hpp"
// #include "dict.hpp"
// #include "command.hpp"
#include "routeMessage.hpp"
#include <memory>
#include <unordered_map>
#include <fstream>
#include <signal.h>

/* 程序启动说明书 */
static void usage(char *proc)
{
    std::cout << "\nUsage:\n\t"
              << proc << " local_port\n\n";
}

/* ./udpServer local_port 运行服务器 */
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    // signal(2, reload);
    // initDict();
    // debugPrint();

    uint16_t port = atoi(argv[1]);

    /* 模拟字典程序 */
    // std::unique_ptr<udpServer> usvr(new udpServer(handlerMessage, port));

    /* 模拟远端终端程序 */
    // std::unique_ptr<udpServer> usvr(new udpServer(execCommand, port));

    /* 模拟聊天室转发消息程序 */
    std::unique_ptr<udpServer> usvr(new udpServer(routeMessage, port));

    usvr->init();
    usvr->start();

    return 0;
}