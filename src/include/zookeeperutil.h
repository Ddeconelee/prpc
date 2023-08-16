#pragma once
#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>
// ��װ��zk�ͻ�����
class ZkClient {
public:
    ZkClient();
    ~ZkClient();
    // ZkClient����������ZkServer
    void Start();
    // ��ZkServer�ϸ���ָ��path����znode�ڵ�
    void Create(const char* path, const char* data, int datalen, int state = 0);
    // ����path��ȡznode�ڵ�ֵ
    std::string GetData(const char* path);
private:
    // zk�Ŀͻ��˾��
    zhandle_t *m_zhandle;
};
