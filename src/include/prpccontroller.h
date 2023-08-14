#pragma once
#include <google/protobuf/service.h>
#include <string>

class PrpcController : public google::protobuf::RpcController {
public:
    PrpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);

    // Ŀǰδʵ�־���Ĺ���
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* Callback);
private:
    bool m_failed;  // RPC����ִ�й����е�״̬
    std::string m_errText; // RPC����ִ�й����еĴ�����Ϣ
};