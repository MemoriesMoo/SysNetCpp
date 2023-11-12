# 共享内存通信示例

## 简介

> 这个示例程序演示了如何使用共享内存进行基本的进程间通信。共享内存是一种允许多个进程共享同一块内存区域的机制，适用于需要高效数据交换的场景。

### 1.获取共享内存键

​	在 `comm.hpp` 文件中，通过 `ftok` 函数获取共享内存键，以确保客户端和服务器可以访问相同的共享内存。

```c++
key_t getKey()
{
    key_t key = ftok(PATH_NAME, PROJ_ID);

    if (key < 0)
    {
        // 如果获取键失败，输出错误信息并退出
        std::cerr << errno << "：" << strerror(errno) << std::endl;
        exit(1);
    }

    return key;
}
```

### 2. 创建/获取共享内存段

通过 `shmget` 函数，根据共享内存键和标志位（`IPC_CREAT`）创建或获取共享内存段。客户端使用 `getShm` 函数，服务器使用 `createShm` 函数。

```cpp
int getShm(key_t key)
{
    return getShmHelper(key, IPC_CREAT);
}

int createShm(key_t key)
{
    return getShmHelper(key, IPC_CREAT | IPC_EXCL | 0666);
}
```

### 3. 连接/分离共享内存

使用 `shmat` 函数连接到共享内存段，获取其起始地址。`attachShm` 函数负责这个过程。

```cpp
void *attachShm(int shmId)
{
    void *mem = shmat(shmId, nullptr, 0);

    if ((long long)mem == -1L)
    {
        std::cerr << "attachShm-> " << errno << "：" << strerror(errno) << std::endl;
        exit(4);
    }

    return mem;
}

void detachShm(void *start)
{
    if (shmdt(start) == -1)
    {
        std::cout << "detachShm-> " << errno << "：" << strerror(errno) << std::endl;
        exit(5);
    }
}
```

### 4. 读写共享内存

客户端通过将消息写入共享内存模拟向服务器发送数据，服务器从共享内存中读取并打印消息模拟接收数据。

```cpp
// 客户端
const char *message = "Hello server!";
while (true)
{
    snprintf(start, MAX_SIZE, "%s[pid:%d][message id:%d]", message, id, cnt);
    sleep(1); // 在发送下一条消息之前休眠一段时间
    cnt++;
}

// 服务器
while (true)
{
    printf("Message from shared memory: %s\n", start);
    sleep(1); // 在读取下一条消息之前休眠一段时间
}
```

## 如何使用

1. **编译程序**：
   - 运行 `make` 命令，将生成 `client` 和 `server` 可执行文件。

2. **运行服务器**：
   - 在一个终端窗口中运行 `./server`。

3. **运行客户端**：
   - 在另一个终端窗口中运行 `./client`。

4. **观察输出**：
   - 客户端不断向共享内存写入消息，而服务器从共享内存中读取并打印消息。

## 注意事项

- 请确保在同一系统上运行客户端和服务器，以确保它们可以访问相同的共享内存。

- 使用 `make clean` 命令可以清理生成的可执行文件。

## 运行效果

- 在运行客户端和服务器之后，可以观察到终端窗口中不断输出的消息，其中包括客户端发送的消息和服务器接收的消息。
