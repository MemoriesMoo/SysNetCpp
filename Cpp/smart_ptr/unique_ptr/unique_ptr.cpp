#include <iostream>

template <typename T>
class unique_ptr {
public:
    // 默认构造函数，创建一个空的unique_ptr
    unique_ptr() : _data(nullptr) {}

    // 带参构造函数，接受一个指向T类型的指针作为参数
    explicit unique_ptr(T* ptr) : _data(ptr) {}

    // 禁用拷贝构造函数，确保对象的独占性
    unique_ptr(const unique_ptr<T>& other) = delete;

    // 禁用拷贝赋值运算符，确保对象的独占性
    unique_ptr<T>& operator=(const unique_ptr<T>& other) = delete;

    // 移动构造函数，接受另一个unique_ptr对象的资源
    unique_ptr(unique_ptr<T>&& other) {
        _data = other._data;     // 移动资源
        other._data = nullptr;   // 重置源对象的资源指针为空
    }

    // 移动赋值运算符，接受另一个unique_ptr对象的资源
    unique_ptr<T>& operator=(unique_ptr<T>&& other) {
        if (this != &other) {
            reset();                // 释放当前资源
            _data = other._data;     // 移动资源
            other._data = nullptr;   // 重置源对象的资源指针为空
        }
        return *this;
    }

    // 返回当前拥有的资源指针
    T* get() const {
        return _data;
    }

    // 释放资源并返回资源指针
    T* release() {
        T* temp = _data;
        _data = nullptr;
        return temp;
    }

    // 释放资源并将资源指针置为空
    void reset() {
        delete _data;
        _data = nullptr;
    }

    // 析构函数，释放资源
    ~unique_ptr() {
        reset();
    }

private:
    T* _data;       // 指向独占的资源的指针
};

int main() {
    // 创建一个unique_ptr并测试其功能

    // 创建一个unique_ptr，拥有一个动态分配的int对象
    unique_ptr<int> ptr1(new int(42));

    // 编译错误：拷贝构造函数被禁用
    // unique_ptr<int> ptr2 = ptr1;

    // 编译错误：赋值运算符被禁用
    // unique_ptr<int> ptr3;
    // ptr3 = ptr1;

    // 使用移动构造函数，ptr1现在为空，ptr4拥有资源
    unique_ptr<int> ptr4 = std::move(ptr1);

    std::cout << "ptr1: " << (ptr1.get() ? "Valid" : "Empty") << std::endl; // 输出：ptr1: Empty
    std::cout << "ptr4: " << (ptr4.get() ? "Valid" : "Empty") << std::endl; // 输出：ptr4: Valid

    // 释放ptr4的资源
    ptr4.reset();

    std::cout << "ptr4: " << (ptr4.get() ? "Valid" : "Empty") << std::endl; // 输出：ptr4: Empty

    return 0;
}
