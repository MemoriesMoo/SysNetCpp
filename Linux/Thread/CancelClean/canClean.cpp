#include <pthread.h>
#include <stdio.h>

void cleanup_function(void *arg)
{
    // 取消清理函数
    printf("Cleanup function called.\n");
}

void *thread_function(void *arg)
{
    // 注册取消清理函数
    pthread_cleanup_push(cleanup_function, NULL);
    // 线程执行的工作
    printf("Thread is executing...\n");

    // 自己取消自己不会触发清理函数
    // pthread_cancel(pthread_self());

    // 弹出取消清理函数
    // 参数为 0 表示弹出时不执行清理函数，其他非零值表示执行清理函数
    pthread_cleanup_pop(0);
    printf("Thread has terminated.\n");
    // 线程终止
    return NULL;
}

int main()
{
    pthread_t thread;
    pthread_create(&thread, NULL, thread_function, (void *)"newThread");

    // 取消创建的子线程
    pthread_cancel(thread);
    // 读者可自行取消此取消函数 观察输出结果

    // 等待线程结束
    pthread_join(thread, NULL);

    return 0;
}
