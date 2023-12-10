#include "udpClient.hpp"
#include <memory>

static void usage(char *proc)
{
    std::cout << "\nUsage:\n\t"
              << proc << " server_ip server_port \n\n";
}

/* ./udpClient server_ip server_port */
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    std::string serverIp = argv[1];
    uint16_t serverPort = atoi(argv[2]);

    std::unique_ptr<udpClient> ucli(new udpClient(serverIp, serverPort));

    ucli->init();
    ucli->run();

    return 0;
}