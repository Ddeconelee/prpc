#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <string>
#include  <functional>
#include <google/protobuf/descriptor.h>
#include <unordered_map>

// ����ṩ�ķ���rpc��������������
class RpcProvider
{
private:
    // �����Eventloop
    muduo::net::EventLoop m_eventloop;

    struct ServiceInfo {
        //����������
        google::protobuf::Service* m_service;
        //������񷽷��ļ�ֵ��
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_method_map;
    };
    // �������ļ�ֵ��
    std::unordered_map<std::string, ServiceInfo> m_service_map;

    // �µ�socket���ӻص�
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    // �ѽ��������û��Ķ�д�¼��ص�
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);

    // Closure�Ļص��������������л�rpc����Ӧ�����緢��
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
    
public:
    // ����ṩ���ⲿʹ�õģ�����ע������ľ�����񲢷���rpc����
    void NotifyService(google::protobuf::Service* service);

    // ����rpc�ڵ㣬�ȴ�rpcԶ�̵���
    void Run();
};
