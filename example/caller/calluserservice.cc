#include <iostream>
#include "prpcapplication.h"
#include "user.pb.h"
#include "prpcchannel.h"

int main(int argc, char** argv) {
    // 调用框架初始化函数
    PrpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new PrpcChannel());

    // 定义具体请求数据
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    // 定义响应数据类型，用来接收响应数据
    fixbug::LoginResponse response;

    // 发起rpc调用，指向 PrpcChannel::CallMethod
    PrpcController controller;
    stub.Login(&controller, &request, &response, NULL); // 这一步是同步阻塞的

    if(controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        // rpc调用完成，读取响应结果
        if(response.result().errcode() == 0) { // 成功
            std::cout << "rpc login response: " << response.success() << std::endl;
        }
        else { // 失败
            std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
        }
    }

    
    fixbug::RegisterRequest req;
    req.set_id(123456);
    req.set_name("li si");
    req.set_pwd("123456");

    // 定义响应数据类型，用来接收响应数据
    fixbug::RegisterResponse resp;

    // 发起rpc调用，指向 PrpcChannel::CallMethod
    controller.Reset();
    stub.Register(&controller, &req, &resp, NULL); // 这一步是同步阻塞的

    if(controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        // rpc调用完成，读取响应结果
        if(resp.result().errcode() == 0) { // 成功
            std::cout << "rpc register response: " << resp.success() << std::endl;
        }
        else { // 失败
            std::cout << "rpc register response error: " << resp.result().errmsg() << std::endl;
        }
    }
    return 0;
}