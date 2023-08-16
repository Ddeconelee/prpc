#include "zookeeperutil.h"
#include "prpcapplication.h"
#include <semaphore.h>
#include <iostream>

/*
zookeeper_mt�����̰߳汾
zookeeper��API�ͻ��˳����ṩ�������߳�
    API�����߳�
    ����I/O�߳� pthread_create poll
    watcher�ص��߳�
*/

// ȫ�ֵ�watcher�۲��� ���ӳɹ��ı�sem��ֵ
void global_watcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx) {
    if(type == ZOO_SESSION_EVENT) {
        if(state == ZOO_CONNECTED_STATE) {
            sem_t* sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem); // 
        }
    }
}

ZkClient::ZkClient() : m_zhandle(NULL) {

}

ZkClient::~ZkClient() {
    if(m_zhandle != NULL) {
        zookeeper_close(m_zhandle);
    }
}

void ZkClient::Start() {
    std::vector<std::string> str = {"rpcserverip", "rpcserverport", "zookeeperip", "zookeeperport"};
    std::string host = PrpcApplication::GetInstance().GetConfig().Load(str[2]);
    std::string port = PrpcApplication::GetInstance().GetConfig().Load(str[3]);
    std::string connstr = host + ":" + port;

    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, NULL, NULL, 0);
    if(!m_zhandle) {
        std::cout << "zookeeper_init error!" << std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem); // ���ź����󶨵�m_zhandle

    sem_wait(&sem); // �ź�������ֱ���ص������ı��ź���
    std::cout << "zookeeper_init success!" << std::endl;
}

void ZkClient::Create(const char* path, const char* data, int datalen, int state) {
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    int flag;
    flag = zoo_exists(m_zhandle, path, 0, NULL);
    if(flag == ZNONODE) {
        flag = zoo_create(m_zhandle, path, data, datalen, 
                                             &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
        if(flag == ZOK) {
            std::cout << "znode create success... path: " << path << std::endl;
        }
        else {
            std::cout << "flag: " << flag << std::endl;
            std::cout << "znode create error... path: " << path << std::endl;
            exit(EXIT_FAILURE);
        }
    }   
}

std::string ZkClient::GetData(const char* path) {
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, NULL);
    if(flag != ZOK) {
        std::cout << "get znode err... path: " << path << std::endl;
        return "";
    }
    else {
        return buffer;
    }
}