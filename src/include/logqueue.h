#pragma once
#include <queue>
#include <thread>
#include <condition_variable>

// �첽��־����
template<typename T>
class LogQueue {
public:
    // ���д
    void Push(const T& data) {
        std::lock_guard<std::mutex> lock(m_mutex); // ����
        m_queue.push(data);
        m_condvar.notify_one();
    }
    // ����������д��txt
    T Pop() {
        std::unique_lock<std::mutex> lock(m_mutex); // ����
        while (m_queue.empty()) {
            // ��־����Ϊ�գ��ȴ�
            m_condvar.wait(lock); // �ͷ���
        }
        T data = m_queue.front();
        m_queue.pop();
        return data;
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvar;
};
