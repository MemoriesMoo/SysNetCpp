# 编译器和编译选项
CC = g++
CXXFLAGS = -std=c++11

# 目标文件和源文件
CLIENT_TARGET = udpClient
SERVER_TARGET = udpServer
CLIENT_SRCS = udpClient.cpp
SERVER_SRCS = udpServer.cpp

# 默认目标
.PHONY: all
all: $(CLIENT_TARGET) $(SERVER_TARGET)

# 编译客户端
$(CLIENT_TARGET): $(CLIENT_SRCS)
	$(CC) -o $@ $^ $(CXXFLAGS)

# 编译服务器
$(SERVER_TARGET): $(SERVER_SRCS)
	$(CC) -o $@ $^ $(CXXFLAGS)

# 清理生成的文件
.PHONY: clean
clean:
	rm -f $(CLIENT_TARGET) $(SERVER_TARGET)
