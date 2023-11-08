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

// ANSI颜色代码
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define NUM 1024
#define OPT_NUM 64
#define HOST_NAME_MAX 256 // 手动定义最大主机名长度
#define PATH_MAX 4096     // 手动定义最大路径长度

char lineCommand[NUM]; // 存储用户输入的命令
char *myargv[OPT_NUM]; // 存储命令参数
int lastCode = 0;      // 最后一条命令的退出状态码
int lastSig = 0;       // 最后一条命令的信号码

// 打印Shell提示符
void print_prompt()
{
    char *username = getlogin(); // 获取当前登录用户名
    if (username == NULL)
    {
        perror("getlogin");
        exit(1);
    }

    char hostname[HOST_NAME_MAX];
    if (gethostname(hostname, sizeof(hostname)) != 0)
    { // 获取主机名
        perror("gethostname");
        exit(1);
    }

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    { // 获取当前工作目录
        perror("getcwd");
        exit(1);
    }

    char *basename_cwd = basename(cwd); // 从完整路径中提取目录名

    // 打印带颜色的提示符
    printf(ANSI_COLOR_RED "[%s" ANSI_COLOR_RESET "@" ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET " %s]$ ", username,
           hostname, basename_cwd);
    fflush(stdout);
}

// 处理输入和输出重定向
void handle_redirection()
{
    int input_fd, output_fd;

    if (strstr(lineCommand, "<"))
    {                                                // 如果命令包含"<"符号
        char *input_file = strtok(lineCommand, "<"); // 分割输入文件
        input_file = strtok(NULL, "<");              // 获取输入文件名
        input_file = strtok(input_file, " ");        // 去除额外的空格
        input_file = strtok(input_file, " ");
        input_file = strtok(input_file, " ");
        input_fd = open(input_file, O_RDONLY); // 打开输入文件
        if (input_fd == -1)
        {
            perror("open");
            exit(1);
        }
        dup2(input_fd, 0); // 用输入文件替换标准输入
        close(input_fd);
    }

    if (strstr(lineCommand, ">"))
    {                                                 // 如果命令包含">"符号
        char *output_file = strtok(lineCommand, ">"); // 分割输出文件
        output_file = strtok(NULL, ">");              // 获取输出文件名
        output_file = strtok(output_file, " ");       // 去除额外的空格
        output_file = strtok(output_file, " ");
        output_file = strtok(output_file, " ");
        output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644); // 打开输出文件
        if (output_fd == -1)
        {
            perror("open");
            exit(1);
        }
        dup2(output_fd, 1); // 用输出文件替换标准输出
        close(output_fd);
    }
}

// 为"ls"命令添加颜色选项
void add_ls_color_option()
{
    int i = 0;
    while (myargv[i] != NULL)
    {
        if (strcmp(myargv[i], "ls") == 0)
        { // 如果命令包含"ls"
            int j = 0;
            while (myargv[j] != NULL)
            {
                j++;
            }
            myargv[j] = (char *)"--color=auto"; // 为"ls"添加颜色选项
            myargv[j + 1] = NULL;
            break;
        }
        i++;
    }
}

int main()
{
    while (1)
    {
        print_prompt();

        char *input = fgets(lineCommand, sizeof(lineCommand) - 1, stdin);
        if (input == NULL)
        {
            perror("fgets");
            exit(1);
        }
        lineCommand[strlen(lineCommand) - 1] = '\0';

        myargv[0] = strtok(lineCommand, " "); // 分割命令和参数
        int i = 1;
        while ((myargv[i++] = strtok(NULL, " ")))
            ;

        if (myargv[0] != NULL)
        {
            if (strcmp(myargv[0], "cd") == 0)
            { // 处理"cd"命令
                if (myargv[1] != NULL)
                {
                    if (chdir(myargv[1]) != 0)
                    {
                        perror("chdir");
                    }
                }
                else
                {
                    fprintf(stderr, "cd: 缺少参数\n");
                }
                continue;
            }

            if (strcmp(myargv[0], "exit") == 0)
            { // 处理"exit"命令
                exit(0);
            }

            if (myargv[0] != NULL && myargv[1] != NULL && strcmp(myargv[0], "echo") == 0)
            { // 处理"echo"命令
                if (strcmp(myargv[1], "$?") == 0)
                {
                    printf("%d, %d\n", lastCode, lastSig); // 输出最后一条命令的退出状态码和信号码
                }
                else
                {
                    printf("%s\n", myargv[1]);
                }
                continue;
            }

            pid_t child_pid = fork(); // 创建子进程
            if (child_pid == -1)
            {
                perror("fork");
                exit(1);
            }

            if (child_pid == 0)
            {                              // 在子进程中
                handle_redirection();      // 处理输入和输出重定向
                add_ls_color_option();     // 为"ls"命令添加颜色选项
                execvp(myargv[0], myargv); // 执行命令
                perror("execvp");
                exit(1);
            }

            int status = 0;
            pid_t ret = waitpid(child_pid, &status, 0); // 等待子进程完成
            if (ret < 0)
            {
                perror("waitpid");
                exit(1);
            }
            lastCode = WEXITSTATUS(status); // 获取子进程的退出状态码
            lastSig = WTERMSIG(status);     // 获取子进程的信号码
        }
    }
}
