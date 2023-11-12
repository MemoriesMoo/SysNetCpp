#include "comm.hpp"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int main()
{
    // 获取共享内存键
    key_t key = getKey();
    printf("key：%d\n", key);

    // 获取或创建共享内存段
    int shmId = getShm(key);
    printf("shmId：%d\n", shmId);

    // 连接到共享内存段
    char *start = (char *)attachShm(shmId);
    printf("连接成功！起始地址：%p。\n", start);

    int messageCounter = 0;

    const char *message = "Hello server!";
    pid_t id = getpid();
    int cnt = 0;
    while (true)
    {
        // 向共享内存段写入消息
        snprintf(start, MAX_SIZE, "%s[pid:%d][message id:%d]", message, id, cnt);
        sleep(1); // 在发送下一条消息之前休眠一段时间
        cnt++;
    }

    // 从共享内存段分离
    detachShm(start);
    printf("分离成功！\n");

    exit(0);
}
