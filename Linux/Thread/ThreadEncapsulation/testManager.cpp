#include "threadManager.h"

void* thread_function(void* arg)
{
    int thread_number = *(static_cast<int*>(arg));
    std::cout << "线程 " << thread_number << " 正在运行。" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "线程 " << thread_number << " 正在工作 (" << i + 1 << "/5)。" << std::endl;
    }
    return nullptr;
}

int main()
{
    const int num_threads = 3;
    Thread* threads[num_threads];

    for (int i = 0; i < num_threads; ++i) {
        threads[i] = new Thread(thread_function, &i, i + 1);
    }

    for (int i = 0; i < num_threads; ++i) {
        threads[i]->join();
        delete threads[i];
    }

    std::cout << "所有线程已完成。" << std::endl;

    return 0;
}
