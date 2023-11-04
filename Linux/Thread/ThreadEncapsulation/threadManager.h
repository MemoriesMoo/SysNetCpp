#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <pthread.h>
#include <memory>
#include <cassert>

class Thread;

// 上下文类，用于传递Thread对象和参数
class Context
{
public:
    Thread* this_; // 指向Thread对象的指针
    void* args_;   // 传递的参数
public:
    Context(Thread* thread, void* args)
        : this_(thread), args_(args) {}
};

class Thread
{
public:
    using func_t = std::function<void*(void*)>;

public:
    Thread(func_t func, void* args = nullptr, int number = 0)
        : func_(func), args_(args)
    {
        // 生成线程名
        char buffer[1024];
        snprintf(buffer, sizeof buffer, "thread-%d", number);
        name_ = buffer;

        // 创建Context对象并传递给pthread_create
        auto ctx = std::unique_ptr<Context>(new Context(this, args));
        int n = pthread_create(&tid_, nullptr, &Thread::start_routine, ctx.get());
        assert(n == 0);
        ctx_ = std::move(ctx);
    }

    // 静态成员函数作为线程入口点
    static void* start_routine(void* args)
    {
        auto ctx = static_cast<Context*>(args);
        void* ret = ctx->this_->run(ctx->args_);
        return ret;
    }

    // 阻塞等待线程结束
    void join()
    {
        int n = pthread_join(tid_, nullptr);
        assert(n == 0);
    }

    // 实际执行线程函数
    void* run(void* args)
    {
        return func_(args);
    }

    // 析构函数确保线程被正确加入
    ~Thread()
    {
        join();
    }

private:
    std::string name_;
    func_t func_;
    void* args_;
    pthread_t tid_;
    std::unique_ptr<Context> ctx_; // 使用智能指针管理上下文对象生命周期
};
