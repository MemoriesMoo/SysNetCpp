#pragma once

#include <iostream>
#include <string>
#include <pthread.h>
#include <functional>
#include <cassert>

class Thread;

class Context
{
public:
    Thread *this_;
    void *args_;

public:
    Context() : this_(nullptr), args_(nullptr) {}
    ~Context() = default;
};

class Thread
{
    typedef std::function<void *(void *)> fun_t;

private:
    pthread_t tid_;    // id
    std::string name_; // 名称
    fun_t func_;       // 执行函数
    void *args_;       // 执行函数参数

    const int bufferLen = 1024;

    // 线程执行函数辅助函数
    void *runHelper(void *args)
    {
        return func_(args);
    }

public:
    Thread(fun_t func, void *args, int number)
        : func_(func), args_(args)
    {
        // 存储线程名称
        char nameBuffer[bufferLen];
        // 名称赋值
        snprintf(nameBuffer, sizeof(nameBuffer) - 1, "thread-%d", number);
        name_ = nameBuffer;
    }

    // 这样可以解决吗start函数混编问题吗？
    // 不行！
    // 类内成员函数会自带隐藏的this指针 缺省参数
    // 不满足pthread_create参数要求
    // void *start_routine(void *args)
    // {
    //     return func_(args);
    // }

    // 解决方法 静态函数？去除对象指针
    // 还是错误 因为静态函数为类所属
    // 无法调用成员方法/成员属性 因此无法使用func_
    // static void *start_routine(void *args)
    // {
    //     return func_(args); // error
    // }

    static void *start_routine(void *args)
    {
        // 类型转换 Context对象中包含对象指针以及需要传递的函数参数
        Context *ctx = static_cast<Context *>(args);
        
        // 接收对应返回值
        void *ret = ctx->this_->runHelper(ctx->args_);
        delete ctx;

        return ret;
    }

    // 线程启动函数
    void start()
    {
        // 原生接口创建线程
        // 此处直接使用func_是C++对象，无法与C语言兼容，如何解决？
        // int res = pthread_create(&tid_, nullptr, func_, args_);

        Context *ctx = new Context();
        ctx->this_ = this;
        ctx->args_ = args_;

        int res = pthread_create(&tid_, nullptr, start_routine, ctx);

        // 确保线程创建成功（debug方式存在 release方式忽略）
        // 意料之外使用if判断或者异常处理
        // 意料之中使用assert强制判断（此处假设不会出现失败情况）
        assert(res == 0);

        // 防止release版本下该变量未使用的warning
        (void)res;
    }

    void join()
    {
        // 原生接口进行线程等待
        int res = pthread_join(tid_, nullptr);

        // 解析同start函数
        assert(res == 0);
        (void)res;
    }
};