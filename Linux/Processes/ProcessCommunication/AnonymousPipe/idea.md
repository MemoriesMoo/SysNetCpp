# 匿名管道

## 概述

该程序实现了一个基本的事件通知系统，使用匿名管道（pipes）实现了父进程和子进程之间的通信。子进程定期发送事件通知，而父进程接收并处理这些通知。

## 代码解析

### 1. 头文件引用

```cpp
#include <iostream>
#include <unistd.h>
#include <cassert>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <ctime>
```

这些头文件包含了所需库，用于输入/输出、进程控制、字符串处理和时间操作。

### 2. 实用函数

#### `getCurrentTime()`

```cpp
std::string getCurrentTime()
```

该函数使用`time`库获取当前时间，并将其格式化为字符串返回。

#### `printColoredMessage()`

```cpp
void printColoredMessage(const std::string& message, const std::string& color)
```

该函数用于以指定颜色打印消息。

### 3. `main()` 函数

#### 3.1 创建管道

```cpp
int fds[2];
int pipe_res = pipe(fds);
assert(pipe_res == 0);
```

在父进程和子进程之间创建一个管道，`fds[0]`为读取端，`fds[1]`为写入端。

#### 3.2 创建子进程

```cpp
pid_t id = fork();
assert(id >= 0);
```

通过`fork()`创建一个子进程。`id`为子进程的进程ID。

#### 3.3 子进程逻辑

```cpp
if (id == 0) {
    // 子进程通信代码
    // ...
}
```

在子进程中，关闭了管道的读取端，并通过循环向父进程发送带有时间戳和事件内容的通知。

#### 3.4 父进程逻辑

```cpp
else {
    // 父进程通信代码
    // ...
}
```

在父进程中，关闭了管道的写入端，并通过循环接收来自子进程的通知。

#### 3.5 等待子进程完成

```cpp
pid_t child_status = waitpid(id, nullptr, 0);
assert(child_status == id);
```

等待子进程完成。

#### 3.6 关闭管道

```cpp
close(fds[0]);
close(fds[1]);
```

关闭管道的读取端和写入端，然后退出父进程。

这个程序演示了父子进程之间如何通过管道进行通信，实现了一个简单的事件通知系统。子进程周期性地发送事件通知，而父进程接收并打印带有时间戳和颜色的消息。