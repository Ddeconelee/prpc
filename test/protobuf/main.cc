#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main() {
    LoginRequest req_a;
    req_a.set_name("zhang san");
    req_a.set_pwd("123456");

    // �������л���������������ת��Ϊ�ַ���
    std::string send_str;
    if (req_a.SerializeToString(&send_str)) {
        std::cout << send_str << std::endl;
    }

    // �����л������ַ����н���������
    LoginRequest req_b;
    if ( req_b.ParseFromString(send_str)) {
        std::cout << req_b.name () << std::endl; 
        std::cout << req_b.pwd () << std::endl; 
    }

    // ��Ӧ����
    LoginResponse rsp;
    ResultCode* rc = rsp.mutable_result(); // ���е���ķ���
    rc->set_errcode(0);
    rc->set_errmsg(""); 
    rsp.set_success(true);

    // ��ȡ�б���Ӧ
    GetFriendListsResponse rsp_friend;
    ResultCode* rc_friend = rsp_friend.mutable_result(); // ���е���ķ���
    rc_friend->set_errcode(0);
    rc_friend->set_errmsg(""); 
    User* user1 = rsp_friend.add_users();
    user1->set_name ("zhangsan");
    user1->set_age(18);
    user1->set_sex(User::Man);
    User* user2 = rsp_friend.add_users();
    user2->set_name ("lisi");
    user2->set_age(20);
    user2->set_sex(User::Woman);

    std::cout << rsp_friend.users_size()  << std::endl; // ��ȡ�б���ֵ�ĸ���
    // User* user3 = rsp_friend.users_list(1);


    return 0;
}