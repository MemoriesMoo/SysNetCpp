当深入了解这段Shell演示代码时，让我们逐步解析它的不同部分，以更详细地了解其工作原理。

### 包含头文件和定义常量

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/utsname.h>
```

上面的代码段包含了所需的C标准库头文件以及其他系统调用相关的头文件。这些头文件包括了在程序中使用的各种函数和类型的声明。

```c
// ANSI color codes
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"
```

在这里定义了一些ANSI颜色代码，用于在终端上生成带颜色的输出。这将在Shell提示符中用于美化显示。

```c
#define NUM 1024
#define OPT_NUM 64
#define HOST_NAME_MAX 256
#define PATH_MAX 4096
```

这些常量用于限制字符串的大小。`NUM` 用于定义命令行输入的最大长度，`OPT_NUM` 用于定义命令参数的最大数量，`HOST_NAME_MAX` 用于定义主机名的最大长度，`PATH_MAX` 用于定义文件路径的最大长度。

### 全局变量

```c
char lineCommand[NUM];
char *myargv[OPT_NUM];
int lastCode = 0;
int lastSig = 0;
```

上述代码定义了一些全局变量，它们将在整个程序中用于存储用户输入的命令、命令参数以及上一个命令的退出状态码和信号码。

### `print_prompt` 函数

```c
void print_prompt()
{
    char *username = getlogin();
    // 获取登录用户名
    if (username == NULL)
    {
        perror("getlogin");
        exit(1);
    }

    char hostname[HOST_NAME_MAX];
    // 获取主机名
    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        perror("gethostname");
        exit(1);
    }

    char cwd[PATH_MAX];
    // 获取当前工作目录
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        exit(1);
    }

    char *basename_cwd = basename(cwd);
    // 从完整路径中提取目录名称

    // 打印带颜色的Shell提示符
    printf(ANSI_COLOR_RED "[%s" ANSI_COLOR_RESET "@" ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET " %s]$ ", username,
           hostname, basename_cwd);
    fflush(stdout);
}
```

`print_prompt` 函数用于显示带颜色的Shell提示符。它执行以下操作：

- 使用 `getlogin` 获取当前登录用户名。
- 使用 `gethostname` 获取主机名。
- 使用 `getcwd` 获取当前工作目录，并使用 `basename` 提取目录名称。
- 使用ANSI颜色代码打印带颜色的Shell提示符，包括用户名、主机名和目录名称。

### `handle_redirection` 函数

```c
void handle_redirection()
{
    int input_fd, output_fd;

    if (strstr(lineCommand, "<"))
    {
        // 如果命令包含 "<" 符号
        char *input_file = strtok(lineCommand, "<");
        // 使用 strtok 分割输入文件
        input_file = strtok(NULL, "<");
        // 获取输入文件名
        input_file = strtok(input_file, " ");
        input_file = strtok(input_file, " ");
        input_file = strtok(input_file, " ");
        // 移除额外的空格
        input_fd = open(input_file, O_RDONLY);
        // 打开输入文件
        if (input_fd == -1)
        {
            perror("open");
            exit(1);
        }
        dup2(input_fd, 0);
        // 用输入文件替换标准输入
        close(input_fd);
    }

    if (strstr(lineCommand, ">"))
    {
        // 如果命令包含 ">" 符号
        char *output_file = strtok(lineCommand, ">");
        // 使用 strtok 分割输出文件
        output_file = strtok(NULL, ">");
        // 获取输出文件名
        output_file = strtok(output_file, " ");
        output_file = strtok(output_file, " ");
        output_file = strtok(output_file, " ");
        // 移除额外的空格
        output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        // 打开输出文件
        if (output_fd == -1)
        {
            perror("open");
            exit(1);
        }
        dup2(output_fd, 1);
        // 用输出文件替换标准输出
        close(output_fd);
    }
}
```

`handle_redirection` 函数用于处理输入和输出重定向。它执行以下操作：

- 如果命令包含 `<` 符号，它会使用 `strtok` 分割命令，以获取输入文件名，然后使用 `open` 打开输入文件，并使用 `dup2` 将其替换为标准输入（文件描述符0）。
- 如果命令包含 `>` 符号，它会使用 `strtok` 分割命令，以获取输出文件名，然后使用 `open` 打开输出文件，并使用 `dup2` 将其替换为标准输出（文件描述符1）。

### `add_ls_color_option`

 函数

```c
void add_ls_color_option()
{
    int i = 0;
    while (myargv[i] != NULL)
    {
        if (strcmp(myargv[i], "ls") == 0)
        {
            int j = 0;
            while (myargv[j] != NULL)
            {
                j++;
            }
            myargv[j] = (char *)"--color=auto";
            myargv[j + 1] = NULL;
            break;
        }
        i++;
    }
}
```

`add_ls_color_option` 函数用于为 `ls` 命令添加颜色选项 `--color=auto`。它执行以下操作：

- 遍历 `myargv` 数组，查找是否存在 `ls` 命令。
- 如果找到 `ls` 命令，它会在参数列表的末尾添加 `--color=auto`，以启用颜色输出。

### `main` 函数

`main` 函数是整个Shell演示的入口点。它包括以下关键部分：

- 使用 `print_prompt` 函数显示带颜色的Shell提示符。
- 使用 `fgets` 获取用户输入的命令，并将其存储在 `lineCommand` 中。然后，使用 `strtok` 分解命令为命令和参数，存储在 `myargv` 中。
- 根据用户输入的命令执行不同的操作，如切换目录、退出Shell或执行外部命令。
- 使用 `fork` 创建子进程来执行外部命令。
- 在子进程中，使用 `execvp` 执行外部命令，并在命令结束后获取退出状态码和信号码。
- 使用 `waitpid` 函数等待子进程的结束。

整体而言，这段代码实现了一个基本的Shell，允许用户执行命令并支持一些常见的Shell功能。但请注意，它仅包括基本功能，没有错误处理和高级特性，仅用于演示和学习目的。它为用户提供了一个简单的命令行界面，可以执行基本的系统命令。