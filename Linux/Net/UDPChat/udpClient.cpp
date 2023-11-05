#include "udpClient.hpp"
#include <thread> // 用于休眠

int main()
{
    // 指定服务器的IP地址和端口号
    std::string serverIp = "127.0.0.1"; // 服务器IP地址
    uint16_t serverPort = 12345;        // 服务器端口号

    // 创建 UDP 客户端对象
    Client::udpClient client(serverIp, serverPort);

    // 初始化客户端
    client.initClient();

    // 准备要发送的数据
    std::string dataToSend = "Hello, Server!";
    int sendInterval = 1; // 发送数据的间隔时间（秒）

    while (true)
    {
        // 发送数据到服务器
        client.sendData(dataToSend);

        // 休眠指定的时间
        std::this_thread::sleep_for(std::chrono::seconds(sendInterval));
    }

    return 0;
}
