#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <unistd.h>

/* 日志等级 */
#define DEBUG 0
#define NORMAL 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

static std::ofstream logFile("logfile.txt", std::ios::app);

std::string logLevelToString(int logLevel)
{
    switch (logLevel)
    {
    case DEBUG:
        return "DEBUG";
    case NORMAL:
        return "NORMAL";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    case FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

void logMessage(int logLevel, const std::string &message)
{
    if (!logFile.is_open())
    {
        std::cerr << "Error opening log file." << std::endl;
        return;
    }

    /* 获取当前时间戳 */
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);

    /* 日志格式 */
    /* [日志等级] [时间戳] [进程PID] [日志具体信息] */
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now);

    logFile << "[" << logLevelToString(logLevel) << "] "
            << "[" << buffer << "] "
            << "[" << getpid() << "] "
            << message << std::endl;
}
