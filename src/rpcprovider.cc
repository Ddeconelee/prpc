#include "rpcprovider.h"
#include "prpcapplication.h"
#include "rpcheader.pb.h"

// ����ṩ���ⲿʹ�õģ�����ע������ľ�����񲢷���rpc����
void RpcProvider::NotifyService(google::protobuf::Service* service) {

    ServiceInfo service_info;
    // ��ȡ��������������Ϣ��ָ��
    const google::protobuf::ServiceDescriptor* service_desc_ptr = service->GetDescriptor();

    // ��ȡ������������
    std::string service_name = service_desc_ptr->name();
    std::cout << "service_name:" << service_name << std::endl;

    // ��ȡ�������ķ�������
    int service_methods_count = service_desc_ptr->method_count();

    // ��ȡ�������ľ��巽��������Ϣ��ָ��
    for(int i = 0; i < service_methods_count; i++) {
        const google::protobuf::MethodDescriptor* method_desc_ptr = service_desc_ptr->method(i);
        //������񷽷���ֵ��
        std::string method_name = method_desc_ptr->name();
        std::cout << "method_name:" << method_name << std::endl;
        service_info.m_method_map.insert({method_name, method_desc_ptr});
    }
    // ����serviceָ��
    service_info.m_service = service;
    // �����������ֵ��
    m_service_map.insert({service_name, service_info});
}

// ����rpc�ڵ㣬�ȴ�rpcԶ�̵���
void RpcProvider::Run() {
    // ��ȡIP��PORT��������address
    std::vector<std::string> str = {"rpcserverip", "rpcserverport", "zookeeperip", "zookeeperport"};
    std::string ip = PrpcApplication::GetInstance().GetConfig().Load(str[0]); // ʵ�������󲢷��ʾ�̬��Ա����m_config
    uint16_t port = atoi(PrpcApplication::GetInstance().GetConfig().Load(str[1]).c_str());
    muduo::net::InetAddress address(ip, port);

    // ����TcpServer����
    muduo::net::TcpServer server(&m_eventloop, address, "Rpcprovider" );

    // �����ӻص�����Ϣ��д�ص� ���������ҵ��
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // ����muduo����߳�����
    server.setThreadNum(4);

    std::cout << "RpcProvider start service at ip: " << ip << " port: " << port << std::endl;

    //  �����������
    server.start();
    // epoll_wait
    m_eventloop.loop(); 
}

// �µ�socket���ӻص�
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
    if(!conn->connected()) { // ��rpc�����ӶϿ�
        conn->shutdown();
    }
}

/* 
RpcProvider��RpcConsumerҪЭ�̺�ͨ����Ҫ�õ���protobuf�������ͣ�
service_name method_name args ������Ӧ��proto��message���ͣ��������ݵ����л��ͷ����л�
�ַ������ݸ�ʽ��
    head_length(4���ֽ�) + header_str + args_str
�ѽ��������û��Ķ�д�¼��ص���Զ��rpc�����������OnMessage��Ӧ
*/
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp) {
    // �����Ͻ��յ�Զ��rpc����������ַ��� Login args
    std::string recv_buf = buffer->retrieveAllAsString();

    // ���ַ����ж�ȡ4���ֽڵ�header_length
    uint32_t header_length = 0;
    recv_buf.copy((char*)&header_length, 4, 0);

    // ����header_length��ȡ����ͷ��ԭʼ�ַ���
    std::string rpc_header_str = recv_buf.substr(4, header_length);
    // �����л�����ͷ
    prpc::RpcHeader rpc_header;
    std::string service_name;
    std::string method_name;
    uint32_t args_length;
    if(rpc_header.ParseFromString(rpc_header_str)) {
        // �����л��ɹ�
        service_name = rpc_header.servic_name();
        method_name = rpc_header.method_name();
        args_length = rpc_header.args_length();
    }
    else {
        // �����л�ʧ��
        std::cout << "rpc_header_str:" << rpc_header_str << "parse error!" << std::endl;
        return;
    }

    // ��ȡrpc�����������ַ�������
    std::string args_str = recv_buf.substr(header_length + 4, args_length);
    
    // ��ӡ������Ϣ
    std::cout << "=======================================" << std::endl;
    std::cout << "header_length:" << header_length << std::endl;
    std::cout << "service_name:" << service_name << std::endl;
    std::cout << "method_name:" << method_name << std::endl;
    std::cout << "args_length:" << args_length << std::endl;
    std::cout << "args_str:" << args_str << std::endl;
    std::cout << "=======================================" << std::endl;

    // ��ȡservice�����method����
    auto it = m_service_map.find(service_name);
    if(it == m_service_map.end()) {
        // �Ҳ�����Ӧ�ľ������
        std::cout << "service_name is not exist!"  << std::endl;
        return;
    }
    auto m_it = it->second.m_method_map.find(method_name);
    if(m_it == it->second.m_method_map.end()) {
        // �Ҳ�����Ӧ�ľ��巽��
        std::cout << "method_name is not exist!" << std::endl;
        return;
    }

    // ��ȡָ��������ĸ���ָ��
    google::protobuf::Service* service = it->second.m_service;
    // ��ȡָ����巽���ĸ���ָ��
    const google::protobuf::MethodDescriptor* method = m_it->second;

    // ����rpc�������õ�����request����Ӧresponse����
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str)) {
        // args����ʧ��
        std::cout << "request parse error! content: " << args_str << std::endl;
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    // �������method�������ã���һ��Closure�Ļص�����
     google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider,
                                                                                                                                                const muduo::net::TcpConnectionPtr&,
                                                                                                                                                google::protobuf::Message*>
                                                                                                                                                (this, 
                                                                                                                                                &RpcProvider::SendRpcResponse,
                                                                                                                                                 conn, response);

    // ����Զ��rpc���󣬵��õ�ǰrpc�ڵ��Ϸ����ķ������������ľ���ʵ��
    // new UserService().Login(Controller, request, response, done)
    service->CallMethod(method, NULL, request, response, done);
}

// Closure�Ļص��������������л�rpc����Ӧ�����緢��
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string response_str;
    if(response->SerializeToString(&response_str)) { //response�������л�
        // ���л��ɹ���ͨ��muduo�⴫�ظ�Զ��rpc
        conn->send(response_str);   
    }

    else {
        std::cout << "serialize response_str error!" << std::endl;
    }
    conn->shutdown(); //ģ�������
}