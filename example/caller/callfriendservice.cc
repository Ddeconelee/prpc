#include <iostream>
#include "prpcapplication.h"
#include "friend.pb.h"


int main(int argc, char** argv) {
    // 调用框架初始化函数
    PrpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法GetFriendsList
    fixbug::FriendServiceRpc_Stub stub(new PrpcChannel());

    // 定义具体请求数据
    fixbug::GetFriendsListRequest request;
    request.set_userid(123);


    // 定义响应数据类型，用来接收响应数据
    fixbug::GetFriendsListResponse response;

    // 定义controller对象
    PrpcController controller;

    // 发起rpc调用，指向 PrpcChannel::CallMethod
    stub.GetFriendsList(&controller, &request, &response, NULL); // 这一步是同步阻塞的

    if(controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        // rpc调用完成，读取响应结果
        if(response.result().errcode() == 0) { // 成功
            std::cout << "rpc GetFriendsList response: " << response.success() << std::endl;
            int size = response.friends_size();
            for(int i  = 0; i < size; i++) {
                std::cout << "index: " << (i + 1) << " name: " << response.friends(i) << std::endl;
            }
        }
        else { // 失败
            std::cout << "rpc GetFriendsList response error: " << response.result().errmsg() << std::endl;
        }
    }
    return 0;
}