#include "comm.hpp"
#include <stdio.h>
#include <unistd.h>

int main()
{
    // 获取共享内存键
    key_t key = getKey();
    printf("key：%d\n", key);

    // 创建新的共享内存段
    int shmId = createShm(key);
    printf("shmId：%d\n", shmId);

    // 连接到共享内存段
    char *start = (char *)attachShm(shmId);
    printf("连接成功！起始地址：%p。\n", start);

    while (true)
    {
        // 从共享内存读取并打印消息
        printf("来自共享内存的消息：%s\n", start);
        sleep(1); // 在读取下一条消息之前休眠一段时间
    }

    // 从共享内存段分离
    detachShm(start);
    printf("分离成功！\n");

    // 删除共享内存段
    delShm(shmId);

    exit(0);
}
