#include <iostream>
#include "prpcapplication.h"
#include "friend.pb.h"


int main(int argc, char** argv) {
    // ���ÿ�ܳ�ʼ������
    PrpcApplication::Init(argc, argv);

    // ��ʾ����Զ�̷�����rpc����GetFriendsList
    fixbug::FriendServiceRpc_Stub stub(new PrpcChannel());

    // ���������������
    fixbug::GetFriendsListRequest request;
    request.set_userid(123);


    // ������Ӧ�������ͣ�����������Ӧ����
    fixbug::GetFriendsListResponse response;

    // ����controller����
    PrpcController controller;

    // ����rpc���ã�ָ�� PrpcChannel::CallMethod
    stub.GetFriendsList(&controller, &request, &response, NULL); // ��һ����ͬ��������

    if(controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        // rpc������ɣ���ȡ��Ӧ���
        if(response.result().errcode() == 0) { // �ɹ�
            std::cout << "rpc GetFriendsList response: " << response.success() << std::endl;
            int size = response.friends_size();
            for(int i  = 0; i < size; i++) {
                std::cout << "index: " << (i + 1) << " name: " << response.friends(i) << std::endl;
            }
        }
        else { // ʧ��
            std::cout << "rpc GetFriendsList response error: " << response.result().errmsg() << std::endl;
        }
    }
    return 0;
}