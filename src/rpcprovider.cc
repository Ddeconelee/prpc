#include "rpcprovider.h"
#include "prpcapplication.h"
#include "rpcheader.pb.h"

// 框架提供给外部使用的，可以注册任意的具体服务并发布rpc方法
void RpcProvider::NotifyService(google::protobuf::Service* service) {

    ServiceInfo service_info;
    // 获取服务对象的描述信息的指针
    const google::protobuf::ServiceDescriptor* service_desc_ptr = service->GetDescriptor();

    // 获取服务对象的名字
    std::string service_name = service_desc_ptr->name();
    std::cout << "service_name:" << service_name << std::endl;

    // 获取服务对象的方法数量
    int service_methods_count = service_desc_ptr->method_count();

    // 获取服务对象的具体方法描述信息的指针
    for(int i = 0; i < service_methods_count; i++) {
        const google::protobuf::MethodDescriptor* method_desc_ptr = service_desc_ptr->method(i);
        //保存服务方法键值对
        std::string method_name = method_desc_ptr->name();
        std::cout << "method_name:" << method_name << std::endl;
        service_info.m_method_map.insert({method_name, method_desc_ptr});
    }
    // 保存service指针
    service_info.m_service = service;
    // 保存服务对象键值对
    m_service_map.insert({service_name, service_info});
}

// 启动rpc节点，等待rpc远程调用
void RpcProvider::Run() {
    // 获取IP和PORT，并传入address
    std::vector<std::string> str = {"rpcserverip", "rpcserverport", "zookeeperip", "zookeeperport"};
    std::string ip = PrpcApplication::GetInstance().GetConfig().Load(str[0]); // 实例化对象并访问静态成员变量m_config
    uint16_t port = atoi(PrpcApplication::GetInstance().GetConfig().Load(str[1]).c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventloop, address, "Rpcprovider" );

    // 绑定连接回调和消息读写回调 分离网络和业务
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);

    std::cout << "RpcProvider start service at ip: " << ip << " port: " << port << std::endl;

    //  启动网络服务
    server.start();
    // epoll_wait
    m_eventloop.loop(); 
}

// 新的socket连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
    if(!conn->connected()) { // 和rpc的连接断开
        conn->shutdown();
    }
}

/* 
RpcProvider和RpcConsumer要协商好通信需要用到的protobuf数据类型：
service_name method_name args 定义相应的proto的message类型，进行数据的序列化和反序列化
字符流数据格式：
    head_length(4个字节) + header_str + args_str
已建立连接用户的读写事件回调，远端rpc服务调用请求，OnMessage响应
*/
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp) {
    // 网络上接收的远程rpc调用请求的字符流 Login args
    std::string recv_buf = buffer->retrieveAllAsString();

    // 从字符流中读取4个字节的header_length
    uint32_t header_length = 0;
    recv_buf.copy((char*)&header_length, 4, 0);

    // 根据header_length读取数据头的原始字符流
    std::string rpc_header_str = recv_buf.substr(4, header_length);
    // 反序列化数据头
    prpc::RpcHeader rpc_header;
    std::string service_name;
    std::string method_name;
    uint32_t args_length;
    if(rpc_header.ParseFromString(rpc_header_str)) {
        // 反序列化成功
        service_name = rpc_header.servic_name();
        method_name = rpc_header.method_name();
        args_length = rpc_header.args_length();
    }
    else {
        // 反序列化失败
        std::cout << "rpc_header_str:" << rpc_header_str << "parse error!" << std::endl;
        return;
    }

    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(header_length + 4, args_length);
    
    // 打印调试信息
    std::cout << "=======================================" << std::endl;
    std::cout << "header_length:" << header_length << std::endl;
    std::cout << "service_name:" << service_name << std::endl;
    std::cout << "method_name:" << method_name << std::endl;
    std::cout << "args_length:" << args_length << std::endl;
    std::cout << "args_str:" << args_str << std::endl;
    std::cout << "=======================================" << std::endl;

    // 获取service对象和method对象
    auto it = m_service_map.find(service_name);
    if(it == m_service_map.end()) {
        // 找不到对应的具体服务
        std::cout << "service_name is not exist!"  << std::endl;
        return;
    }
    auto m_it = it->second.m_method_map.find(method_name);
    if(m_it == it->second.m_method_map.end()) {
        // 找不到对应的具体方法
        std::cout << "method_name is not exist!" << std::endl;
        return;
    }

    // 获取指向具体服务的父类指针
    google::protobuf::Service* service = it->second.m_service;
    // 获取指向具体方法的父类指针
    const google::protobuf::MethodDescriptor* method = m_it->second;

    // 生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str)) {
        // args解析失败
        std::cout << "request parse error! content: " << args_str << std::endl;
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    // 给下面的method方法调用，绑定一个Closure的回调函数
     google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider,
                                                                                                                                                const muduo::net::TcpConnectionPtr&,
                                                                                                                                                google::protobuf::Message*>
                                                                                                                                                (this, 
                                                                                                                                                &RpcProvider::SendRpcResponse,
                                                                                                                                                 conn, response);

    // 根据远端rpc请求，调用当前rpc节点上发布的方法，即方法的具体实现
    // new UserService().Login(Controller, request, response, done)
    service->CallMethod(method, NULL, request, response, done);
}

// Closure的回调操作，用于序列化rpc的响应和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string response_str;
    if(response->SerializeToString(&response_str)) { //response进行序列化
        // 序列化成功，通过muduo库传回给远端rpc
        conn->send(response_str);   
    }

    else {
        std::cout << "serialize response_str error!" << std::endl;
    }
    conn->shutdown(); //模拟短连接
}