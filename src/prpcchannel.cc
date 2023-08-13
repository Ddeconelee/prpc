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
/*
Լ���ķ��ͽ������ݸ�ʽ��
header_length(4���ֽ�) + service_name + method_name + args_length +args
*/

void PrpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                                                                google::protobuf::RpcController* controller, 
                                                                const google::protobuf::Message* request,
                                                                google::protobuf::Message* response, 
                                                                google::protobuf::Closure* done)  {
    
    const google::protobuf::ServiceDescriptor* service_ptr = method->service();
    std::string service_name = service_ptr->name();
    std::string method_name = method->name();

    // ��ȡ���������л��ַ������� args_length
    uint32_t args_length = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str)) {
        args_length = args_str.size();
    }
    else {
        std::cout << "serialize request error : " << std::endl;
        return;
    }

    // ����rpc������header
    prpc::RpcHeader rpc_header;
    rpc_header.set_servic_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_length(args_length);

    uint32_t header_length = 0;
    std::string rpc_header_str;
    if(rpc_header.SerializeToString(&rpc_header_str)) { // rpc_header���л�
        header_length = rpc_header_str.size(); // rpc_header_str����
    }
    else {
        std::cout << "serialize rpc header error!" << std::endl;
        return;
    }

    // ��֯�����͵�rpc������ַ���
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_length, 4));
    send_rpc_str += rpc_header_str; // ����rpc_header_str���л�����
    send_rpc_str += args_str;// ����args_str���л�����

    // ��ӡ������Ϣ
    std::cout << "=======================================" << std::endl;
    std::cout << "header_length:" << header_length << std::endl;
    std::cout << "service_name:" << service_name << std::endl;
    std::cout << "method_name:" << method_name << std::endl;
    std::cout << "args_length:" << args_length << std::endl;
    std::cout << "args_str:" << args_str << std::endl;
    std::cout << "=======================================" << std::endl;

    // ʹ��tcp��̣����rpc������Զ�̵���
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd == -1) {
        std::cout << "create socket errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // ��ȡip��port
    std::vector<std::string> str = {"rpcserverip", "rpcserverport", "zookeeperip", "zookeeperport"};
    std::string ip = PrpcApplication::GetInstance().GetConfig().Load(str[0]); // ʵ�������󲢷��ʾ�̬��Ա����m_config
    uint16_t port = atoi(PrpcApplication::GetInstance().GetConfig().Load(str[1]).c_str());
    // ����socket����
    struct sockaddr_in server_addr;  
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // ����rpc����ڵ�
    if(connect(clientfd, (struct sockaddr*)& server_addr, sizeof(server_addr)) == -1) {
        std::cout << "client socket error ! errno: " << errno << std::endl;
        close(clientfd);
        exit(EXIT_FAILURE);
    }

    // ����rpc����
    if(send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1) {
        std::cout << "send error ! errno: " << errno << std::endl;
        close(clientfd);
        return;
    }

    //����rpc�������Ӧ
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if((recv_size = recv(clientfd, recv_buf, 1024, 0)) == -1) {
         std::cout << "recv error ! errno: " << errno << std::endl;
         close(clientfd);
        return;
    }

    if(!response->ParseFromArray(recv_buf, recv_size)) {
        std::cout << "parse error! response_str: " << recv_buf << std::endl;
        close(clientfd);
        return;
    }
    close(clientfd);
}