#include <iostream>
#include <string> 
#include "friend.pb.h"
#include "prpcapplication.h"
#include "rpcprovider.h"
#include <vector>


class FriendService : public fixbug::FriendServiceRpc { // rpc服务提供者继承FriendServiceRpc，实现具体的Login功能
public:
    std::vector<std::string> GetFriendsList(uint32_t user_id) { //本地GetFriendsList具体实现
        std::cout << "doing local service: GetFriendsList" << std::endl;
        std::cout << "user_id:" << user_id << std::endl;
        std::vector<std::string> vec;
        vec.push_back("Tom");
        vec.push_back("Jerry");
        return vec;
    }

    // 重写基类FriendServiceRpc的虚函数，等待服务端RPC_Stub的调用，也就是RPC最右侧的实现代码
    void GetFriendsList(::google::protobuf::RpcController* controller,
                                                const ::fixbug::GetFriendsListRequest* request, // 从这里获取请求参数
                                                ::fixbug::GetFriendsListResponse* response, // 从这里传入回传参数
                                                ::google::protobuf::Closure* done) 
    {
        // 从rpc框架的请求中获取请求参数
        uint32_t user_id = request->userid(); 
        std::vector<std::string> friends_list = GetFriendsList(user_id);

        // 回传响应参数
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        for(std::string &name : friends_list) {
            std::string *p = response->add_friends();
            *p = name;
        }
        
        response->set_success(true); 

        // 执行回调函数，后续由框架序列化并传给客户端
        done->Run(); // 目前是纯虚函数，还没实现具体功能
    }
};

 int main(int argc, char** argv) {
    // 调用框架的初始化操作
    PrpcApplication::Init(argc, argv);

    // 把FriendService对象注册并发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    // 启动一个rpc服务发布节点，进入阻塞状态，等待其他rpc节点远程调用
    provider.Run();

    return 0;
}

