#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>

// 函数指针（回调）
typedef void (*TaskCallback)();

// 示例任务1
void task1()
{
    printf("Task 1 executed.\n");
}

// 示例任务2
void task2()
{
    printf("Task 2 executed.\n");
}

// 示例任务3
void task3()
{
    printf("Task 3 executed.\n");
}

// 示例任务4
void task4()
{
    printf("Task 4 executed.\n");
}

// 示例任务5
void task5()
{
    printf("Task 5 executed.\n");
}

int main(int argc, char *argv[])
{
    int num_tasks = 5;        // 默认任务数量
    int polling_interval = 2; // 默认轮询间隔

    // 解析命令行参数
    int opt;
    while ((opt = getopt(argc, argv, "n:i:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            num_tasks = atoi(optarg); // 从命令行参数设置任务数量
            break;
        case 'i':
            polling_interval = atoi(optarg); // 从命令行参数设置轮询间隔
            break;
        default:
            fprintf(stderr, "Usage: %s [-n num_tasks] [-i polling_interval]\n", argv[0]);
            exit(1);
        }
    }

    // 创建任务函数指针数组
    TaskCallback tasks[] = {task1, task2, task3, task4, task5};

    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (child_pid == 0)
    {
        // 子进程的代码
        while (1)
        {
            printf("I' am child process (running), pid: %d, ppid: %d\n", getpid(), getppid());
            sleep(polling_interval); // 等待指定的轮询间隔
        }

        exit(42); // 子进程退出并返回状态
    }
    else
    {
        // 父进程的代码
        printf("Parent process (PID %d) waiting for child process (PID %d) to exit...\n", getpid(), child_pid);

        int status;
        int result;

        // 非阻塞轮询等待
        while (1)
        {
            result = waitpid(child_pid, &status, WNOHANG); // 非阻塞等待子进程退出

            if (result == child_pid)
            {
                // 子进程已退出
                if (WIFEXITED(status))
                {
                    int exit_status = WEXITSTATUS(status);
                    printf("Child process (PID: %d) exited with code: %d\n", child_pid, exit_status);
                }
                else
                {
                    int signal_status = WTERMSIG(status);
                    printf("Child process (PID: %d) exited with signal: %d\n", child_pid, signal_status);
                }

                break;
            }
            else if (result == 0)
            {
                // 子进程仍在运行，父进程可执行其他任务
                printf("Child process is running, parent process doing other tasks...\n");

                // 执行任务函数
                for (int i = 0; i < num_tasks; i++)
                {
                    tasks[i]();
                }

                sleep(polling_interval); // 等待指定的轮询间隔
            }
            else
            {
                perror("waitpid");
                break;
            }
        }

        // 在这里可以执行其他任务
        printf("Parent process completed other tasks...\n");

        return 0;
    }
}
