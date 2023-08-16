#include "prpcchannel.h"
#include <string>
#include "rpcheader.pb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "prpcapplication.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "prpccontroller.h"
#include "zookeeperutil.h"

/*
约定的发送接收数据格式：
header_length(4个字节) + service_name + method_name + args_length +args
*/


void PrpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                                                                google::protobuf::RpcController* controller, 
                                                                const google::protobuf::Message* request,
                                                                google::protobuf::Message* response, 
                                                                google::protobuf::Closure* done)  {
    
    const google::protobuf::ServiceDescriptor* service_ptr = method->service();
    std::string service_name = service_ptr->name();
    std::string method_name = method->name();

    // 获取参数的序列化字符串长度 args_length
    uint32_t args_length = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str)) {
        args_length = args_str.size();
    }
    else {
        controller->SetFailed("serialize request error!");
        return;
    }

    // 定义rpc的请求header
    prpc::RpcHeader rpc_header;
    rpc_header.set_servic_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_length(args_length);

    uint32_t header_length = 0;
    std::string rpc_header_str;
    if(rpc_header.SerializeToString(&rpc_header_str)) { // rpc_header序列化
        header_length = rpc_header_str.size(); // rpc_header_str长度
    }
    else {
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    // 组织待发送的rpc请求的字符流
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_length, 4));
    send_rpc_str += rpc_header_str; // 加上rpc_header_str序列化数据
    send_rpc_str += args_str;// 加上args_str序列化数据

    // 打印调试信息
    std::cout << "=======================================" << std::endl;
    std::cout << "header_length:" << header_length << std::endl;
    std::cout << "service_name:" << service_name << std::endl;
    std::cout << "method_name:" << method_name << std::endl;
    std::cout << "args_length:" << args_length << std::endl;
    std::cout << "args_str:" << args_str << std::endl;
    std::cout << "=======================================" << std::endl;

    // 使用tcp编程，完成rpc方法的远程调用
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd == -1) {
        std::cout << "create socket error! errno: " << errno << std::endl;
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno: %d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 获取ip和port
    std::vector<std::string> str = {"rpcserverip", "rpcserverport", "zookeeperip", "zookeeperport"};
    // std::string ip = PrpcApplication::GetInstance().GetConfig().Load(str[0]); // 实例化对象并访问静态成员变量m_config
    // uint16_t port = atoi(PrpcApplication::GetInstance().GetConfig().Load(str[1]).c_str());

    //rpc查询zk上具体服务的znode节点的值
    ZkClient zk_cli;
    zk_cli.Start();
    //UserServiceRpc/Login
    std::string method_path = "/" + service_name + "/" + method_name;
    // 127.0.0.1:8000
    std::string host_data = zk_cli.GetData(method_path.c_str());
    if(host_data == "") {
        controller->SetFailed(method_path + " is not exist!");
        return;
    }
    int idx = host_data.find(":");
    if(idx == -1) {
        controller->SetFailed(method_path + "address is not valid!");
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint32_t port = atoi(host_data.substr(idx + 1, host_data.size() - idx).c_str());

    // 配置socket参数
    struct sockaddr_in server_addr;  
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if(connect(clientfd, (struct sockaddr*)& server_addr, sizeof(server_addr)) == -1) {
        std::cout << "client socket error! errno: " << errno << std::endl;
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "client socket error! errno: %d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 发送rpc请求
    if(send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1) {
        std::cout << "send error! errno: " << errno << std::endl;
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno: %d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    //接收rpc请求的响应
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if((recv_size = recv(clientfd, recv_buf, 1024, 0)) == -1) {
         std::cout << "recv error! errno: " << errno << std::endl;
         close(clientfd);
         char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno: %d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    if(!response->ParseFromArray(recv_buf, recv_size)) {
        std::cout << "parse error! response_str: " << recv_buf << std::endl;
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "parse error! response_str: %s", recv_buf);
        controller->SetFailed(errtxt);
        return;
    }
    close(clientfd);
}