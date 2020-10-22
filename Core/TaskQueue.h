#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <queue>
#include <mutex>
#include <atomic>
#include <set>
#include <condition_variable>

#include "Task.h"

namespace XSPlayer {
    struct DelayTask {
        std::chrono::high_resolution_clock::time_point delay;
        TaskPtr pTask;
        bool operator<(const DelayTask& task) const {
            return task.delay.time_since_epoch() < this->delay.time_since_epoch();
        }
    };

        class TaskQueue {
        public:
            TaskQueue();
            bool Push(const TaskPtr& pTask, long long delay = 0);
            void Stop();
            bool IsRunning() const { return m_bRunning; }
            TaskPtr Pull(void);
            TaskPtr PullDelay(void);
            bool IsEmpty() const;
            size_t Size() const;

        private:
            using ListTasks = std::queue<TaskPtr>;
            using ListDelayTasks = std::set<DelayTask>;
            std::mutex m_mutex;
            std::condition_variable m_condition;
            ListTasks m_listTasks;
            ListDelayTasks m_listDelayTasks;
            std::atomic_bool m_bRunning = true;
        };

        using TaskQueuePtr = std::shared_ptr<TaskQueue>;
}  // namespace XSPlayer

#endif // TASKQUEUE_H
