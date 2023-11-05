#include "udpServer.hpp"
#include <memory>

using namespace Server;

int main()
{
    // 指定服务器端口号和可选的IP地址
    uint16_t serverPort = 12345;
    std::string serverIp = "0.0.0.0"; // 默认是"0.0.0.0"，即绑定所有可用的网络接口

    // 创建 UDP 服务器对象
    Server::udpServer server(serverPort, serverIp);

    // 初始化服务器
    server.initServer();

    // 启动服务器，处理接收到的数据包
    server.start();

    return 0;
}
