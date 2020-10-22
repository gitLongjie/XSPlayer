#include "TaskQueue.h"

#include <chrono>

#include "Core/Log.h"

namespace XSPlayer {
TaskQueue::TaskQueue() {}

bool TaskQueue::Push(const TaskPtr& pTask, long long delay) {
    LOG_ENTER;
    if (!IsRunning()) {
        return false;
    }

    if (0 == delay) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_listTasks.push(pTask);
    }
    else {
        std::unique_lock<std::mutex> lock(m_mutex);
        auto tp = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(delay);
        auto now = std::chrono::high_resolution_clock::now();
        auto delay = now - tp;
        long long count = std::chrono::duration_cast<std::chrono::milliseconds>(delay).count();
        DelayTask task = {tp, pTask};
        m_listDelayTasks.insert(task);
    }
  
  m_condition.notify_one();
  LOG_LIVE;
  return true;
}

void TaskQueue::Stop() {
    LOG_ENTER;
    m_bRunning = false;
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_listTasks.empty()) {
        m_listTasks.pop();
    }
    m_condition.notify_all();
    LOG_LIVE;
}

TaskPtr TaskQueue::Pull(void) {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    if (m_listTasks.empty()) {
        LOGI("stop");
        return nullptr;
    }

    TaskPtr pTask = m_listTasks.front();
    m_listTasks.pop();
    return pTask;
}

TaskPtr TaskQueue::PullDelay(void) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_listDelayTasks.empty()) {
        m_condition.wait(lock, [this] {
            return !m_bRunning.load()
                || !this->m_listTasks.empty() 
                || !this->m_listDelayTasks.empty();
        });
        return nullptr;
    }
    else {
        if (m_listDelayTasks.empty()) {
            return nullptr;
        }

        auto itor = m_listDelayTasks.begin();
        auto now = std::chrono::high_resolution_clock::now();
        if (now > itor->delay) {
            TaskPtr pTask = itor->pTask;
            m_listDelayTasks.erase(itor);
            return std::move(pTask);
        }

        m_condition.wait_until(lock, itor->delay, [this] {
            return !m_bRunning.load() || !this->m_listTasks.empty();
        });
    }
    return nullptr;
    

}

bool TaskQueue::IsEmpty() const {
    return m_listTasks.empty();
}

size_t TaskQueue::Size() const {
    return m_listTasks.size();
}

}  // namespace XSPlayer
