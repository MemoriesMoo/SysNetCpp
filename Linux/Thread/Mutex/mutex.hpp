#pragma once

#include <iostream>
#include <pthread.h>

// 互斥锁类，用于管理pthread互斥锁
class Mutex
{
public:
    // 构造函数，可选参数为指向现有互斥锁的指针
    Mutex(pthread_mutex_t *lock_p = nullptr) : lock_p_(lock_p) {}

    // 如果互斥锁非空，则锁定互斥锁
    void lock()
    {
        if (lock_p_)
            pthread_mutex_lock(lock_p_);
    }

    // 解锁互斥锁，如果互斥锁非空
    void unlock()
    {
        if (lock_p_)
            pthread_mutex_unlock(lock_p_);
    }

    // 默认析构函数
    ~Mutex() = default;

private:
    // 指向pthread互斥锁的指针
    pthread_mutex_t *lock_p_;
};

// 锁守卫类，用于自动加锁和解锁
class LockGuard
{
public:
    // 构造函数，在构造时加锁
    LockGuard(pthread_mutex_t *mutex) : mutex_(mutex)
    {
        mutex_.lock();
    }

    // 析构函数，在析构时解锁
    ~LockGuard()
    {
        mutex_.unlock();
    }

private:
    // 互斥锁对象
    Mutex mutex_;
};
