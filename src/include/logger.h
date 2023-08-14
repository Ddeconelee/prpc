#pragma once
#include "logqueue.h"
#include <string>

enum LogLevel {
    INFO, // 普通日志信息
    ERROR, // 错误信息


};

//prpc框架提供的日志系统
class Logger {
public:
    // 获取日志的单例
    static Logger& GetInstance();
    // 设置日志级别
    void SetLogLevel(LogLevel level);
    // 写日志
    void Log(std::string msg);
private:
    int m_loglevel; // 记录日志级别 
    LogQueue<std::string> m_logqueue;

    // 单例模式
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};

// 定义宏 
#define LOG_INFO(logmsgformat, ...)  \
    do  \
    {  \
        Logger & logger = Logger::GetInstance();  \
        logger.SetLogLevel(INFO);  \
        char c[1024] = {0};  \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__);  \
        logger.Log(c);  \
    }  while  (0)

#define LOG_ERR(logmsgformat, ...)  \
    do  \
    {  \
        Logger & logger = Logger::GetInstance();  \
        logger.SetLogLevel(ERROR);  \
        char c[1024] = {0};  \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__);  \
        logger.Log(c);  \
    }  while  (0)
    