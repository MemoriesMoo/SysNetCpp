#include <iostream>
#include <unistd.h>
#include <cassert>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <ctime>

// Function to get the current time as a string
std::string getCurrentTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-d %X", &tstruct);
    return buf;
}

// Function to print a message with a specified color
void printColoredMessage(const std::string& message, const std::string& color) {
    std::cout << "\033[" << color << "m" << message << "\033[0m" << std::endl;
}

int main()
{
    int fds[2];

    // Create a pipe
    int pipe_res = pipe(fds);
    assert(pipe_res == 0);

    pid_t id = fork();
    assert(id >= 0);

    if (id == 0)
    {
        // In the child process

        // Close the read end of the pipe
        close(fds[0]);

        // Child process communication code
        const char *str = "I'm the child process";
        int cnt = 0;
        while (true)
        {
            char buffer[1024];
            // Send an event notification from child to parent with a timestamp and custom event content
            snprintf(buffer, sizeof(buffer), "Child[%d]: Event - Type: Alert, Description: Security breach detected", cnt++);
            std::string message = getCurrentTime() + " | Type: Event | Sender: Child | " + buffer;
            write(fds[1], message.c_str(), message.length());
            // Write once per second
            sleep(4);
        }

        // Close the write end of the pipe and exit the child process
        close(fds[1]);
        exit(0);
    }

    // In the parent process

    // Close the write end of the pipe
    close(fds[1]);

    // Parent process communication code
    int cnt = 0;
    while (true)
    {
        char buffer[1024];
        // Receive an event notification from the child
        ssize_t s = read(fds[0], buffer, sizeof(buffer) - 1);

        if (s > 0)
        {
            buffer[s] = '\0';
            // Print the event notification with timestamp, colored output, and sender information
            std::string receivedMessage = std::string(buffer);
            std::string timestamp = getCurrentTime();
            std::string senderInfo = receivedMessage.substr(receivedMessage.find("Sender:") + 8);
            std::string eventType = receivedMessage.substr(receivedMessage.find("Type:") + 6, 9);
            std::string eventDescription = receivedMessage.substr(receivedMessage.find("Description:") + 13);
            std::string coloredMessage = "Parent[" + std::to_string(cnt++) + "]: " + buffer + " (" + timestamp + ")";
            printColoredMessage(coloredMessage, "32");  // 32 represents green color
            std::cout << "   Event Type: " << eventType << std::endl;
            std::cout << "   Event Description: " << eventDescription << std::endl;
            std::cout << "   Sender: " << senderInfo << std::endl;
        }
    }

    // Wait for the child process to complete
    pid_t child_status = waitpid(id, nullptr, 0);
    assert(child_status == id);

    // Close the read end of the pipe and exit the parent process
    close(fds[0]);
    return 0;
}
 