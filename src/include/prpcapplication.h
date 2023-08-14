#pragma once
#include "prpcconfig.h"
#include "prpcchannel.h"
#include "prpccontroller.h"


// prpc框架的基础类，单例模式
class PrpcApplication {
public:
    static void Init(int argc, char** argv);
    static PrpcApplication& GetInstance();
    static PrpcConfig& GetConfig();
private:
    static PrpcConfig m_config;
    PrpcApplication(){}
    // 去掉拷贝构造函数
    PrpcApplication(const PrpcApplication&) = delete; 
    PrpcApplication(PrpcApplication&&) = delete;
};