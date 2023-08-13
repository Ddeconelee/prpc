#include "prpcconfig.h"
#include <iostream>
#include <string>

// 负责解析加载配置文件
void PrpcConfig::LoadConfigFile(const char* config_file) {
  FILE *pf = fopen(config_file, "r");
  if(!pf) {
    std::cout << config_file << "is not exist!" << std::endl;
    exit(EXIT_FAILURE);
  }

  // 1.注释 2.正确的配置项 = 3.去掉开头多余的空格
  while(!feof(pf)) {
    char buf[512] = {0};
    fgets(buf, 512, pf);

    // 去掉字符串前面多余的空格
    std::string read_buf(buf);
    
    // 去掉多余的空格
    Trim(read_buf);

    // 判断#的注释
    if(read_buf[0] == '#' || read_buf.empty()) {
      continue;
    }
    // 解析配置项
    int idx = read_buf.find('=');
    if(idx == -1) {
      // 配置项不合法
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

// 查询配置项信息
std::string PrpcConfig::Load(std::string& key) {
    auto it = m_config_map.find(key);
    if(it == m_config_map.end()) {
      return "";
    }
    return it->second;
}

// 去掉前后字符串
void PrpcConfig::Trim(std::string& src_buf) {
  int idx = src_buf.find_first_not_of(' ');
    if(idx != -1) { // 前面有空格
      src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }

    // 去掉字符串后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if(idx != -1) { // 后面有空格
      src_buf = src_buf.substr(0, idx + 1);
    }  
}