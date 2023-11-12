# 进程池模拟

这个C++程序模拟了一个简单的进程池，用于演示多进程通信、信号处理、任务分配和资源管理的基本概念。

## 主要功能

### 1. 进程池创建

程序创建了一个包含多个子进程的进程池。每个子进程负责执行特定的任务。

### 2. 任务分配

每个子进程都有一个独立的任务执行函数，分别模拟了下载任务、IO任务和刷新任务。这些任务函数是用户定义的，可以根据实际需求进行修改和扩展。

```cpp
void downloadTask()
{
    // 模拟下载任务
    sleep(1);
    std::cout << "进程[" << getpid() << "] 正在执行下载任务!\n";
}

void ioTask()
{
    // 模拟IO任务
    sleep(1);
    std::cout << "进程[" << getpid() << "] 正在执行IO任务!\n";
}

void flushTask()
{
    // 模拟刷新任务
    sleep(1);
    std::cout << "进程[" << getpid() << "] 正在执行刷新任务!\n";
}
```

### 3. 任务派发

主进程通过随机选择子进程和任务的方式，将任务派发给选中的子进程。这展示了基本的任务调度和进程通信。

```cpp
int main()
{
    // ...（省略其他代码）

    while (true)
    {
        int subIndex = distribution(generator); // 随机选择子进程
        int taskIndex = distribution(generator); // 随机选择任务

        sendTask(pool[subIndex], taskIndex); // 将任务发送给选中的子进程
        sleep(3); // 等待一段时间，模拟任务的执行
    }

    // ...（省略其他代码）
}
```

### 4. 信号处理

程序注册了信号处理函数，捕获Ctrl+C信号(SIGINT)。当用户终止程序时，信号处理函数负责清理资源并等待所有子进程退出。

```cpp
// 信号处理函数
void signalHandler(int signum)
{
    std::cout << "接收到信号：" << signum << "\n";
    cleanupResources(pool);
    exit(signum);
}

int main()
{
    // ...（省略其他代码）

    // 注册信号处理函数，捕获Ctrl+C信号(SIGINT)
    signal(SIGINT, signalHandler);

    // ...（省略其他代码）
}
```

## 如何运行

确保你的环境中有支持C++11的编译器（如g++）。然后执行以下步骤：

1. **编译代码**:

    ```bash
    g++ -o a.out yourfilename.cpp
    ```

2. **运行可执行文件**:

    ```bash
    ./a.out
    ```

## 注意事项

- 该程序演示了基本概念，实际使用中可能需要更多的错误处理和边界情况的考虑。
