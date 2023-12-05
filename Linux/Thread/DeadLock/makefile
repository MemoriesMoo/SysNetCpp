# 编译器
CXX = g++

# 编译选项
CXXFLAGS = -std=c++11 -pthread

# 目标文件
TARGET = deadlock_example

# 源文件
SRCS = deadLock.cpp

# 生成目标
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# 清理生成的目标文件
clean:
	rm -f $(TARGET)
