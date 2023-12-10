# 设置 C++ 编译器
CXX := g++
# 设置 C++ 编译标志
CXXFLAGS := -std=c++11 -lpthread

# 申明虚拟目标
.PHONY: all clean

# 默认目标：构建 udpClient 和 udpServer
all: udpClient udpServer

# 构建 udpClient 目标
udpClient: udpClient.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# 构建 udpServer 目标
udpServer: udpServer.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# 清理生成的可执行文件
clean:
	rm -f udpClient udpServer
