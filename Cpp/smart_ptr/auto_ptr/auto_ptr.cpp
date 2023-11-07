#include <iostream>

template <typename T>
class auto_ptr {
public:
    auto_ptr(T* ptr = nullptr) : _ptr(ptr) {}

    // 移动构造函数，接受右值引用
    auto_ptr(auto_ptr<T>&& other) {
        _ptr = other._ptr;
        other._ptr = nullptr;
    }

    // 移动赋值运算符，接受右值引用
    auto_ptr<T>& operator=(auto_ptr<T>&& other) {
        if (this != &other) {
            reset();  // 释放当前资源
            _ptr = other._ptr;
            other._ptr = nullptr;
        }
        return *this;
    }

    // 获取指针
    T* get() const {
        return _ptr;
    }

    // 释放资源
    void reset() {
        delete _ptr;
        _ptr = nullptr;
    }

    // 解引用操作符
    T& operator*() const {
        return *_ptr;
    }

    // 成员访问操作符
    T* operator->() const {
        return _ptr;
    }

    // 析构函数，释放资源
    ~auto_ptr() {
        reset();
    }

private:
    T* _ptr;
};

int main() {
    auto_ptr<int> ptr1(new int(42));
    auto_ptr<int> ptr2 = std::move(ptr1);

    if (ptr1.get() == nullptr) {
        std::cout << "ptr1 is empty" << std::endl;
    }

    if (ptr2.get() != nullptr) {
        std::cout << "ptr2 contains: " << *ptr2 << std::endl;
    }

    return 0;
}
