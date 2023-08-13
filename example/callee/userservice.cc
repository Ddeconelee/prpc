#include <iostream>
#include <string> 
#include "user.pb.h"
#include "prpcapplication.h"
#include "rpcprovider.h"


class UserService : public fixbug::UserServiceRpc { // rpc�����ṩ�߼̳�UserServiceRpc��ʵ�־����Login����
public:
    bool Login(std::string name, std::string pwd) { //����Login����ʵ��
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name:" << name << "pwd:" << pwd << std::endl;
        return true;
    }

    // ��д����UserServiceRpc���麯�����ȴ������RPC_Stub�ĵ��ã�Ҳ����RPC���Ҳ��ʵ�ִ���
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,  // �������ȡ�������
                       ::fixbug::LoginResponse* response, // �����ﴫ��ش�����
                       ::google::protobuf::Closure* done) 
    {
        // ��rpc��ܵ������л�ȡ�������
        std::string name = request->name(); 
        std::string pwd = request->pwd();

        bool login_result = Login(name, pwd); // ���б���ʵ��

        // �ش���Ӧ����
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result); 

        // ִ�лص������������ɿ�����л��������ͻ���
        done->Run(); // Ŀǰ�Ǵ��麯������ûʵ�־��幦��
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
    // ��rpc��ܵ������л�ȡ�������
        uint32_t id = request->id();
        std::string name = request->name(); 
        std::string pwd = request->pwd();

        bool register_result = Register(id, name, pwd); // ���б���ʵ��

        // �ش���Ӧ����
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(register_result); 

        // ִ�лص������������ɿ�����л��������ͻ���
        done->Run(); // Ŀǰ�Ǵ��麯������ûʵ�־��幦��
    }
};

int main(int argc, char** argv) {
    // ���ÿ�ܵĳ�ʼ������
    PrpcApplication::Init(argc, argv);

    // ��UserService����ע�Ტ������rpc�ڵ���
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // ����һ��rpc���񷢲��ڵ㣬��������״̬���ȴ�����rpc�ڵ�Զ�̵���
    provider.Run();

    return 0;
}