#include "comm.hpp"

int main()
{
    std::cout << "客户端启动！\n";

    // 以只写方式打开命名管道
    int wfd = open(NAMED_PIPE, O_WRONLY);

    // 检查文件描述符是否有效
    if (wfd < 0)
        exit(1);

    while (true)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        std::cout << "请输入: ";
        fgets(buffer, sizeof(buffer), stdin);

        // 去掉输入字符串的换行符
        if (strlen(buffer) > 0)
            buffer[strlen(buffer) - 1] = 0;

        // 将用户输入写入命名管道
        ssize_t writeBytes = write(wfd, buffer, strlen(buffer));

        // 使用断言确保写入成功
        assert(writeBytes == strlen(buffer));
        (void)writeBytes; // 防止未使用的变量警告
    }

    // 关闭文件描述符
    close(wfd);
}
