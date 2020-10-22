#include "ThreadPoolImpl.h"

#include <cmath>

#include "Core/Log.h"
#include "TaskQueue.h"
#include "Core/ThreadPool.h"

namespace XSPlayer {

    class Thread : public std::enable_shared_from_this<Thread>{
    public:
        Thread(ThreadPoolImpl* pThreadPool) : m_pThreadPool(pThreadPool){
        }

        bool Run() {
            m_thread = std::thread(std::bind(&Thread::DoWorkTask, shared_from_this()));
            return true;
        }

        std::thread::id GetThreadId() {
            return m_thread.get_id();
        }

        void Join(void) {
            return m_thread.join();
        }

        std::thread::id RunTask(const TaskPtr& task, long long delay = 0) {
            m_taskQueue.Push(task, delay);
            return GetThreadId();
        }

        bool IsEmpty(void) const {
            return m_taskQueue.IsEmpty();
        }

        size_t GetTaskQueueSize() const {
            return m_taskQueue.Size();
        }

        void Stop(void) {
            m_taskQueue.Stop();
        }

        bool IsIdle(void) const { return m_taskQueue.IsEmpty() && m_bIdle; }

    private:
        void DoWorkTask() {
            LOG_ENTER;
            while (m_taskQueue.IsRunning()) {
                auto pTask = m_taskQueue.Pull();
                if (nullptr != pTask) {
                    m_bIdle = false;
                    pTask->OnRun();
                    m_bIdle = true;
                }

                pTask = m_taskQueue.PullDelay();
                if (nullptr != pTask) {
                    m_bIdle = false;
                    pTask->OnRun();
                    m_bIdle = true;
                }
            }
            LOG_LIVE;
        }
        
    private:
        std::thread m_thread;
        TaskQueue m_taskQueue;
        ThreadPoolImpl* m_pThreadPool;
        bool m_bIdle = true;
    };

ThreadPoolImpl::ThreadPoolImpl(ThreadType type, size_t nThreadSize /*= 2 */) : m_type(type) {
    LOG_ENTER;
    nThreadSize = nThreadSize < 1 ? 1 : nThreadSize;

  for (size_t index = 0; index < nThreadSize; ++index) {
      ThreadPtr pThread = std::make_shared<Thread>(this);
      pThread->Run();
      m_threads.push_back(std::move(pThread));
  }
  LOG_LIVE;
}

ThreadPoolImpl::~ThreadPoolImpl() {
    LOG_ENTER;
    Stop();
    LOG_LIVE;
}

std::thread::id ThreadPoolImpl::PushTask(const TaskPtr& task, long long delay, const std::thread::id& threadId/* = std::thread::id()*/) {
    LOG_ENTER;

    if (ThreadPool::IsInvalideThreadId(threadId)){
        
        std::lock_guard<std::mutex> lock(m_mutex);
        for (size_t index = 0; index < m_threads.size(); ++index) {
            if (m_threads[index]->IsIdle()) {
                LOG_LIVE;
                return m_threads[index]->RunTask(task, delay);
            }
        }

        size_t minTaskQueueLenght = INT_MAX;
        int minFlag = 0;
        for (size_t index = 0; index < m_threads.size(); ++index) {
            if (m_threads[index]->GetTaskQueueSize() < minTaskQueueLenght) {
                minFlag = index;
            }            
        }
        LOG_LIVE;
        return m_threads[minFlag]->RunTask(task, delay);
    }
    else {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (size_t index = 0; index < m_threads.size(); ++index) {
            if (m_threads[index]->GetThreadId() == threadId) {
                LOG_LIVE;
                return m_threads[index]->RunTask(task, delay);
            }
        }
    }

    LOG_LIVE;
    return ThreadPool::InvalidThreadId();
}

void ThreadPoolImpl::Stop(void) {
    LOG_ENTER;
  
    for (size_t index = 0; index < m_threads.size(); ++index) {
        m_threads[index]->Stop();
    }

    for (size_t index = 0; index < m_threads.size(); ++index) {
        m_threads[index]->Join();
    }

    m_threads.clear();
  LOG_LIVE;
}

size_t ThreadPoolImpl::GetIdleCount(void) const {
    size_t count = 0;
    for (auto& item : m_threads) {
        count += item->GetTaskQueueSize();
    }
    return count;
}

// bool ThreadPoolImpl::IsRunning() const {
//     return m_taskQueue.IsRunning();
// }

IOThreadPoolPtr ThreadPoolImplFactory::CreateIOThreadPool() {
    IOThreadPoolPtr ioThreadPool(new ThreadPoolImpl(ThreadType::TT_IO, 1));
    return ioThreadPool;
}

UnkownThreadPoolPtr ThreadPoolImplFactory::CreateUnkownPool() {
    IOThreadPoolPtr unkownThreadPool(new ThreadPoolImpl(ThreadType::TT_UNKNOWN));
    return unkownThreadPool;
}

DecodecThreadPoolPtr ThreadPoolImplFactory::CreateDecodecPool() {
    DecodecThreadPoolPtr pDecodecThreadPool(new ThreadPoolImpl(ThreadType::TT_DECODE));
    return pDecodecThreadPool;
}

}
