#pragma once
#include "prpcconfig.h"
#include "prpcchannel.h"
#include "prpccontroller.h"


// prpc��ܵĻ����࣬����ģʽ
class PrpcApplication {
public:
    static void Init(int argc, char** argv);
    static PrpcApplication& GetInstance();
    static PrpcConfig& GetConfig();
private:
    static PrpcConfig m_config;
    PrpcApplication(){}
    // ȥ���������캯��
    PrpcApplication(const PrpcApplication&) = delete; 
    PrpcApplication(PrpcApplication&&) = delete;
};