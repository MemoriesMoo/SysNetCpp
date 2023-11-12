# 将 "all" 目标声明为伪目标
.PHONY: all

# 定义 "all" 目标，其依赖于 "client" 和 "server"
all: client server

# 从 "shm_client.cpp" 编译 "client" 程序
client: client.cpp
	g++ -o $@ $^ -std=c++11

# 从 "shm_server.cpp" 编译 "server" 程序
server: server.cpp
	g++ -o $@ $^ -std=c++11

# 将 "clean" 目标声明为伪目标
.PHONY: clean

# 定义 "clean" 目标以删除已编译的二进制文件
clean:
	rm -f server client
