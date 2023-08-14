#include "prpccontroller.h"

PrpcController::PrpcController() {
    m_failed = false;
    m_errText = "";
}

void PrpcController::Reset() {
    m_failed = false;
    m_errText = "";
}

bool PrpcController::Failed() const {
    return m_failed;
}

std::string PrpcController::ErrorText() const {
    return m_errText;
}

void PrpcController::SetFailed(const std::string& reason) {
    m_failed = true;
    m_errText = reason;
}

// Ŀǰδʵ�־���Ĺ���
void PrpcController::StartCancel() {}
bool PrpcController::IsCanceled() const {return false;}
void PrpcController::NotifyOnCancel(google::protobuf::Closure* Callback) {}