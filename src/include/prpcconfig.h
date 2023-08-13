#pragma once

#include <unordered_map>
#include <string>

// rpcserviceip rpcserviceport zookeeperip zookeeperport
// ��ܶ�ȡ�����ļ���
class PrpcConfig {
private:
    std::unordered_map<std::string, std::string> m_config_map;

public:
    // ����������������ļ�
    void LoadConfigFile(const char* config_file);
    // ��ѯ��������Ϣ
    std::string Load(std::string& key);
    void Trim(std::string& src_buf);
};