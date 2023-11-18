## 代码解析

### 信号处理函数

```cpp
void handle_sigint(int)
{
    std::cout << "接收到 Ctrl+C，信号已经被捕捉！\n";

    // 在这里可以执行一些清理操作，然后退出程序
    // exit(0);
}
```

在`Unix/Linux`系统中，信号是一种进程间通信的机制，用于通知进程发生了某个事件。当接收到特定信号时，操作系统会执行注册的信号处理函数。在这里，`handle_sigint`函数被注册为捕捉`Ctrl+C`信号(`SIGINT`)的处理函数。当用户按下`Ctrl+C`时，系统会发送`SIGINT`信号，触发此函数的执行。在这个函数中，你可以执行一些清理操作，然后选择退出程序。

### 信号屏蔽与恢复

```cpp
sigset_t block, oblock, pending;
sigemptyset(&block);   // 清空屏蔽集合
sigemptyset(&oblock);  // 清空旧屏蔽集合
sigemptyset(&pending); // 清空挂起集合

sigaddset(&block, BLOCK_SIG); // 添加要屏蔽的信号

// 屏蔽信号并保存旧的掩码
if (sigprocmask(SIG_SETMASK, &block, &oblock) == -1)
{
    perror("sigprocmask"); // 打印错误信息并退出
    return 1;
}
```

#### 信号屏蔽

信号屏蔽是一种机制，用于暂时阻止某些信号的传递给进程。在这里，`sigprocmask`函数用于将`BLOCK_SIG`信号添加到屏蔽集合中，意味着在执行关键代码段时，该信号将被暂时屏蔽，不会中断程序执行。屏蔽信号是为了确保在执行关键代码段时，不会因为信号中断而导致不一致的状态。

### 主循环

```cpp
int cnt = 10;
while (true)
{
    // 获取挂起的信号
    sigemptyset(&pending);
    if (sigpending(&pending) == -1)
    {
        perror("sigpending"); // 打印错误信息并退出
        break;
    }

    // 打印挂起的信号
    show_pending(pending);

    // 休眠1秒
    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (cnt-- == 0)
    {
        std::cout << "恢复旧的信号掩码" << std::endl;
        // 恢复旧的信号掩码
        if (sigprocmask(SIG_SETMASK, &oblock, nullptr) == -1)
        {
            perror("sigprocmask"); // 打印错误信息并退出
            return 1;
        }
    }
}
```

#### 挂起的信号处理

- 获取挂起的信号集合：使用 `sigpending` 函数获取当前挂起的信号集合，并将其存储在 `pending` 变量中。
- 打印挂起的信号：通过调用 `show_pending` 函数，将挂起的信号集合以可读的形式打印出来。这有助于了解在屏蔽期间有哪些信号被发送。

#### 信号屏蔽的恢复

当 `cnt` 计数减为零时，程序会执行以下操作：

- 打印信息：输出 "恢复旧的信号掩码"，表示即将恢复之前被屏蔽的信号。
- 恢复旧的信号掩码：通过调用 `sigprocmask` 函数，将之前保存的旧屏蔽集合 `oblock` 恢复，即将之前被屏蔽的信号重新允许传递给进程。

### 代码解析总结

该程序通过信号处理函数捕获Ctrl+C信号，通过信号屏蔽机制确保在关键代码段执行时不被中断，同时通过检查挂起的信号了解在屏蔽期间有哪些信号被发送。通过定期恢复旧的信号掩码，程序保证之前被屏蔽的信号能够重新生效。