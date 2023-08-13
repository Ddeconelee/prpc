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

// 框架提供的发布rpc服务的网络对象类
class RpcProvider
{
private:
    // 组合了Eventloop
    muduo::net::EventLoop m_eventloop;

    struct ServiceInfo {
        //保存服务对象
        google::protobuf::Service* m_service;
        //保存服务方法的键值对
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_method_map;
    };
    // 保存服务的键值对
    std::unordered_map<std::string, ServiceInfo> m_service_map;

    // 新的socket连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    // 已建立连接用户的读写事件回调
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);

    // Closure的回调操作，用于序列化rpc的响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
    
public:
    // 框架提供给外部使用的，可以注册任意的具体服务并发布rpc方法
    void NotifyService(google::protobuf::Service* service);

    // 启动rpc节点，等待rpc远程调用
    void Run();
};
