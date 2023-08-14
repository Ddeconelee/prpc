#include <iostream>
#include "logger.h"
#include <time.h>

// 获取日志的单例
Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
    // 启动专门的写日志线程
    std::thread WriteLogTask([&](){ //lambda表达式
        for(;;) {
            // 获取当天的日期，取日志信息，写入相应的日志文件
            time_t now = time(NULL);
            tm *nowtm = localtime(&now);

            // 文件名
            char file_name[128] = {0};
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);

            //
            FILE *pf = fopen(file_name, "a+");
            if(pf == NULL) {
                std::cout << "logger file: " << file_name << "open fail!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_logqueue.Pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d => [%s]", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec, (m_loglevel == INFO ? "info" : "error"));
            msg.insert(0, time_buf);
            msg.append("\n");

            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    // 设置分离线程
    WriteLogTask.detach();
}
// 设置日志级别
void Logger::SetLogLevel(LogLevel level) {
    m_loglevel = level;
}

// 写日志
void Logger::Log(std::string msg) {
    m_logqueue.Push(msg);
}