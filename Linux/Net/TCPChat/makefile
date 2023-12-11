# 设置 C++ 编译器
CXX := g++
# 设置 C++ 编译标志
CXXFLAGS := -std=c++11 -lpthread

# 申明虚拟目标
.PHONY: all clean

# 默认目标：构建 tcpClient 和 tcpServer
all: tcpClient tcpServer

# 构建 tcpClient 目标
tcpClient: tcpClient.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# 构建 tcpServer 目标
tcpServer: tcpServer.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# 清理生成的可执行文件
clean:
	rm -f tcpClient tcpServer
