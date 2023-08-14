#pragma once
#include <queue>
#include <thread>
#include <condition_variable>

// 异步日志队列
template<typename T>
class LogQueue {
public:
    // 多个写
    void Push(const T& data) {
        std::lock_guard<std::mutex> lock(m_mutex); // 加锁
        m_queue.push(data);
        m_condvar.notify_one();
    }
    // 单个读，并写入txt
    T Pop() {
        std::unique_lock<std::mutex> lock(m_mutex); // 加锁
        while (m_queue.empty()) {
            // 日志队列为空，等待
            m_condvar.wait(lock); // 释放锁
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
