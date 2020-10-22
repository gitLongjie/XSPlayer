#include "Core/AppHandleTaskQueue.h"

#include "Core/ThreadPool.h"

#include "Core/Log.h"

namespace XSPlayer{

    AppHandleTaskQueue::AppHandleTaskQueue() : m_pTaskQueue(new TaskQueue) {
        LOG_ENTER;
        if (nullptr == m_pTaskQueue) {
            return;
        }
//         m_pTaskQueue->SetThreadId(std::this_thread::get_id());
//         m_pTaskQueue->Start();

        LOG_LIVE;
    }

    AppHandleTaskQueue::~AppHandleTaskQueue(void) {

    }

    std::thread::id AppHandleTaskQueue::PushTask(const TaskPtr& task, long long delay,
                                                 const std::thread::id& threadId) {
        LOG_ENTER;
        if (nullptr == m_pTaskQueue) {
            return ThreadPool::InvalidThreadId();
        }

        bool result = m_pTaskQueue->Push(task);
        LOG_LIVE;
        return ThreadPool::InvalidThreadId();
    }

    void AppHandleTaskQueue::Stop() {
        LOG_ENTER;
        if (nullptr == m_pTaskQueue) {
            return;
        }

        m_pTaskQueue->Stop();
        LOG_LIVE;
    }
// 
//     bool AppHandleTaskQueue::IsRunning() const {
//         LOG_ENTER;
//         if (nullptr == m_pTaskQueue) {
//             return false;
//         }
// 
//         bool result = m_pTaskQueue->IsRunning();
//         LOG_LIVE;
//         return result;
//     }

    ThreadType AppHandleTaskQueue::GetType(void) {
        return ThreadType::TT_MAIN;
    }


    size_t AppHandleTaskQueue::GetIdleCount(void) const {
        return m_pTaskQueue->IsEmpty() ? 1 : 0;
    }

    void AppHandleTaskQueue::OnRun() {
        if (nullptr == m_pTaskQueue) {
            return;
        }

//        m_pTaskQueue->TranslateTask();
    }
}