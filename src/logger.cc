#include <iostream>
#include "logger.h"
#include <time.h>

// ��ȡ��־�ĵ���
Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
    // ����ר�ŵ�д��־�߳�
    std::thread WriteLogTask([&](){ //lambda���ʽ
        for(;;) {
            // ��ȡ��������ڣ�ȡ��־��Ϣ��д����Ӧ����־�ļ�
            time_t now = time(NULL);
            tm *nowtm = localtime(&now);

            // �ļ���
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
    // ���÷����߳�
    WriteLogTask.detach();
}
// ������־����
void Logger::SetLogLevel(LogLevel level) {
    m_loglevel = level;
}

// д��־
void Logger::Log(std::string msg) {
    m_logqueue.Push(msg);
}