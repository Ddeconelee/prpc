#include <iostream>
#include <string> 
#include "friend.pb.h"
#include "prpcapplication.h"
#include "rpcprovider.h"
#include <vector>


class FriendService : public fixbug::FriendServiceRpc { // rpc�����ṩ�߼̳�FriendServiceRpc��ʵ�־����Login����
public:
    std::vector<std::string> GetFriendsList(uint32_t user_id) { //����GetFriendsList����ʵ��
        std::cout << "doing local service: GetFriendsList" << std::endl;
        std::cout << "user_id:" << user_id << std::endl;
        std::vector<std::string> vec;
        vec.push_back("Tom");
        vec.push_back("Jerry");
        return vec;
    }

    // ��д����FriendServiceRpc���麯�����ȴ������RPC_Stub�ĵ��ã�Ҳ����RPC���Ҳ��ʵ�ִ���
    void GetFriendsList(::google::protobuf::RpcController* controller,
                                                const ::fixbug::GetFriendsListRequest* request, // �������ȡ�������
                                                ::fixbug::GetFriendsListResponse* response, // �����ﴫ��ش�����
                                                ::google::protobuf::Closure* done) 
    {
        // ��rpc��ܵ������л�ȡ�������
        uint32_t user_id = request->userid(); 
        std::vector<std::string> friends_list = GetFriendsList(user_id);

        // �ش���Ӧ����
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        for(std::string &name : friends_list) {
            std::string *p = response->add_friends();
            *p = name;
        }
        
        response->set_success(true); 

        // ִ�лص������������ɿ�����л��������ͻ���
        done->Run(); // Ŀǰ�Ǵ��麯������ûʵ�־��幦��
    }
};

 int main(int argc, char** argv) {
    // ���ÿ�ܵĳ�ʼ������
    PrpcApplication::Init(argc, argv);

    // ��FriendService����ע�Ტ������rpc�ڵ���
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    // ����һ��rpc���񷢲��ڵ㣬��������״̬���ȴ�����rpc�ڵ�Զ�̵���
    provider.Run();

    return 0;
}

