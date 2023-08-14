#include <iostream>
#include "prpcapplication.h"
#include "user.pb.h"
#include "prpcchannel.h"

int main(int argc, char** argv) {
    // ���ÿ�ܳ�ʼ������
    PrpcApplication::Init(argc, argv);

    // ��ʾ����Զ�̷�����rpc����Login
    fixbug::UserServiceRpc_Stub stub(new PrpcChannel());

    // ���������������
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    // ������Ӧ�������ͣ�����������Ӧ����
    fixbug::LoginResponse response;

    // ����rpc���ã�ָ�� PrpcChannel::CallMethod
    PrpcController controller;
    stub.Login(&controller, &request, &response, NULL); // ��һ����ͬ��������

    if(controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        // rpc������ɣ���ȡ��Ӧ���
        if(response.result().errcode() == 0) { // �ɹ�
            std::cout << "rpc login response: " << response.success() << std::endl;
        }
        else { // ʧ��
            std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
        }
    }

    
    fixbug::RegisterRequest req;
    req.set_id(123456);
    req.set_name("li si");
    req.set_pwd("123456");

    // ������Ӧ�������ͣ�����������Ӧ����
    fixbug::RegisterResponse resp;

    // ����rpc���ã�ָ�� PrpcChannel::CallMethod
    controller.Reset();
    stub.Register(&controller, &req, &resp, NULL); // ��һ����ͬ��������

    if(controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        // rpc������ɣ���ȡ��Ӧ���
        if(resp.result().errcode() == 0) { // �ɹ�
            std::cout << "rpc register response: " << resp.success() << std::endl;
        }
        else { // ʧ��
            std::cout << "rpc register response error: " << resp.result().errmsg() << std::endl;
        }
    }
    return 0;
}