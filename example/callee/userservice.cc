#include <iostream>
#include <string> 
#include "user.pb.h"
#include "prpcapplication.h"
#include "rpcprovider.h"


class UserService : public fixbug::UserServiceRpc { // rpc服务提供者继承UserServiceRpc，实现具体的Login功能
public:
    bool Login(std::string name, std::string pwd) { //本地Login具体实现
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name:" << name << "pwd:" << pwd << std::endl;
        return true;
    }

    // 重写基类UserServiceRpc的虚函数，等待服务端RPC_Stub的调用，也就是RPC最右侧的实现代码
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,  // 从这里获取请求参数
                       ::fixbug::LoginResponse* response, // 从这里传入回传参数
                       ::google::protobuf::Closure* done) 
    {
        // 从rpc框架的请求中获取请求参数
        std::string name = request->name(); 
        std::string pwd = request->pwd();

        bool login_result = Login(name, pwd); // 进行本地实现

        // 回传响应参数
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result); 

        // 执行回调函数，后续由框架序列化并传给客户端
        done->Run(); // 目前是纯虚函数，还没实现具体功能
    }


    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "id: " << id << " name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }

    void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done) 
{
    // 从rpc框架的请求中获取请求参数
        uint32_t id = request->id();
        std::string name = request->name(); 
        std::string pwd = request->pwd();

        bool register_result = Register(id, name, pwd); // 进行本地实现

        // 回传响应参数
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(register_result); 

        // 执行回调函数，后续由框架序列化并传给客户端
        done->Run(); // 目前是纯虚函数，还没实现具体功能
    }
};

int main(int argc, char** argv) {
    // 调用框架的初始化操作
    PrpcApplication::Init(argc, argv);

    // 把UserService对象注册并发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动一个rpc服务发布节点，进入阻塞状态，等待其他rpc节点远程调用
    provider.Run();

    return 0;
}