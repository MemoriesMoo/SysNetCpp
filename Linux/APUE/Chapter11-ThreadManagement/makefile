# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -std=c99 -lpthread

# Source file
SRC = error.c 11-4-pthread_exit参数使用不正确.c

# Executable name
TARGET = myprogram

# Default rule to build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Clean rule to remove the executable
clean:
	rm -f $(TARGET)
