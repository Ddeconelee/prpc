#include "prpcconfig.h"
#include <iostream>
#include <string>

// ����������������ļ�
void PrpcConfig::LoadConfigFile(const char* config_file) {
  FILE *pf = fopen(config_file, "r");
  if(!pf) {
    std::cout << config_file << "is not exist!" << std::endl;
    exit(EXIT_FAILURE);
  }

  // 1.ע�� 2.��ȷ�������� = 3.ȥ����ͷ����Ŀո�
  while(!feof(pf)) {
    char buf[512] = {0};
    fgets(buf, 512, pf);

    // ȥ���ַ���ǰ�����Ŀո�
    std::string read_buf(buf);
    
    // ȥ������Ŀո�
    Trim(read_buf);

    // �ж�#��ע��
    if(read_buf[0] == '#' || read_buf.empty()) {
      continue;
    }
    // ����������
    int idx = read_buf.find('=');
    if(idx == -1) {
      // ������Ϸ�
      continue;
    }

    std::string key;
    std::string value;
    key = read_buf.substr(0, idx);
    Trim(key);
    int end_idx = read_buf.find('\n', idx);

    value = read_buf.substr(idx + 1, end_idx - idx - 1);
    Trim(value);
    m_config_map.insert({key, value});
  }
}

// ��ѯ��������Ϣ
std::string PrpcConfig::Load(std::string& key) {
    auto it = m_config_map.find(key);
    if(it == m_config_map.end()) {
      return "";
    }
    return it->second;
}

// ȥ��ǰ���ַ���
void PrpcConfig::Trim(std::string& src_buf) {
  int idx = src_buf.find_first_not_of(' ');
    if(idx != -1) { // ǰ���пո�
      src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }

    // ȥ���ַ����������Ŀո�
    idx = src_buf.find_last_not_of(' ');
    if(idx != -1) { // �����пո�
      src_buf = src_buf.substr(0, idx + 1);
    }  
}