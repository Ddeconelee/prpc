#include "prpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>

PrpcConfig PrpcApplication::m_config;

void ShowArgsHelp() {
    std::cout << "command -i <configfile>" << std::endl;
}

void PrpcApplication::Init(int argc, char** argv) {
    // 加载配置文件前的判断
    if (argc < 2) {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1) {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    //加载配置文件 rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
    m_config.LoadConfigFile(config_file.c_str());
    std::vector<std::string> str = {"rpcserverip", "rpcserverport", "zookeeperip", "zookeeperport"};
    // std::cout << "rpcserverip:" << m_config.Load(str[0]) << std::endl;
    // std::cout << "rpcserverport:" << m_config.Load(str[1]) << std::endl;
    // std::cout << "zookeeperip:" << m_config.Load(str[2]) << std::endl;
    // std::cout << "zookeeperport:" << m_config.Load(str[3]) << std::endl;
}

PrpcApplication& PrpcApplication::GetInstance() {
    static PrpcApplication app;
    return app;
}

PrpcConfig& PrpcApplication::GetConfig() {
    return m_config;
}