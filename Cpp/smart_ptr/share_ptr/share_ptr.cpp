#include <iostream>

template <typename T>
class share_ptr
{
public:
    // 默认构造函数
    share_ptr()
        : _data(nullptr),    // 初始化共享数据指针为空
          _refCount(nullptr) // 初始化引用计数为空
    {
    }

    // 带参构造函数，接受一个指向T类型的指针作为参数
    share_ptr(T *ptr)
        : _data(ptr),
          _refCount(new size_t(1))
    {
    }

    // 拷贝构造函数，接受另一个share_ptr对象作为参数
    share_ptr(const share_ptr<T> &other)
        : _data(other._data),
          _refCount(other._refCount)
    {
        if (_refCount)
        {
            (*_refCount)++;
        }
    }

    // 重载赋值运算符，用于将另一个share_ptr对象赋值给当前对象
    share_ptr<T> &operator=(const share_ptr<T> &other)
    {
        if (this != &other)
        {
            release(); // 释放当前资源

            _data = other._data;
            _refCount = other._refCount;

            if (_refCount)
            {
                (*_refCount)++;
            }
        }

        return *this;
    }

    // 返回共享数据指针
    T *get()
    {
        return _data;
    }

    // 返回引用计数，如果引用计数为空则返回0
    size_t useCount()
    {
        return _refCount ? (*_refCount) : 0;
    }

    // 析构函数，用于释放资源
    ~share_ptr()
    {
        release();
    }

private:
    T *_data;          // 指向共享的数据
    size_t *_refCount; // 引用计数指针

    // 释放资源的私有函数
    void release()
    {
        if (_refCount)
        {
            (*_refCount)--;

            if (*_refCount == 0) // 如果引用计数变为0
            {
                delete _data;
                delete _refCount;
            }
        }
    }
};

int main()
{
    // 创建一个share_ptr并测试useCount
    share_ptr<int> ptr1(new int(42));
    std::cout << "ptr1 useCount: " << ptr1.useCount() << std::endl; // 输出：1

    // 复制构造函数，共享同一个资源
    share_ptr<int> ptr2 = ptr1;
    std::cout << "ptr1 useCount: " << ptr1.useCount() << std::endl; // 输出：2
    std::cout << "ptr2 useCount: " << ptr2.useCount() << std::endl; // 输出：2

    // 重载赋值运算符，共享同一个资源
    share_ptr<int> ptr3;
    ptr3 = ptr2;
    std::cout << "ptr1 useCount: " << ptr1.useCount() << std::endl; // 输出：3
    std::cout << "ptr2 useCount: " << ptr2.useCount() << std::endl; // 输出：3
    std::cout << "ptr3 useCount: " << ptr3.useCount() << std::endl; // 输出：3

    return 0;
}
