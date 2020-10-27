#include "ThreadPool.h"
#include "ThreadPoolImpl.h"
#include "Log.h"

namespace XSPlayer {

template <>
ThreadPool* Singleton<ThreadPool>::m_pSingleton = nullptr;

ThreadPool::ThreadPool() {
    LOG_ENTER;
    RegistThreadPoolInner(ThreadPoolImplFactory::CreateIOThreadPool());
    RegistThreadPoolInner(ThreadPoolImplFactory::CreateUnkownPool());
    RegistThreadPoolInner(ThreadPoolImplFactory::CreateDecodecPool());
    LOG_LIVE;
}

ThreadPool::~ThreadPool() {
    LOG_ENTER;
    UnregistTaskQueue(ThreadType::TT_IO);
    UnregistTaskQueue(ThreadType::TT_UNKNOWN);
    UnregistTaskQueue(ThreadType::TT_DECODE);
    LOG_LIVE;
}

void ThreadPool::RegistThreadPoolInner(const IThreadPoolInnerPtr& threadPoolInner) {
    LOG_ENTER;
    if (nullptr == threadPoolInner) {
        return;
    }

    std::unique_lock<std::mutex> lock(m_mutex);
    m_listThreadPoolInner[threadPoolInner->GetType()] = threadPoolInner;
    LOG_LIVE;
}

void ThreadPool::UnregistTaskQueue(const IThreadPoolInnerPtr& threadPoolInner) {
    LOG_ENTER;
    if (nullptr == threadPoolInner) {
        return;
    }

    UnregistTaskQueue(threadPoolInner->GetType());
    LOG_LIVE;
}

void ThreadPool::UnregistTaskQueue(ThreadType type) {
    LOG_ENTER;
    IThreadPoolInnerPtr pThreadPool;
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        auto itor = m_listThreadPoolInner.find(type);
        if (m_listThreadPoolInner.end() == itor) {
            return;
        }
        pThreadPool = itor->second;
        m_listThreadPoolInner.erase(itor);
    }
    LOG_LIVE;
}

std::thread::id g_invalideThreadId;
const std::thread::id& ThreadPool::InvalidThreadId(void) {
    return g_invalideThreadId;
}

bool ThreadPool::IsInvalideThreadId(const std::thread::id& threadId) {
    return g_invalideThreadId == threadId;
}

std::thread::id ThreadPool::PushTask(const TaskPtr& task, const std::thread::id& threadId,
                                     long long delay, ThreadType type /*= UNKOWN*/) {
    return ThreadPool::GetSingleton().PushTaskImpl(task, threadId, delay, type);
}

std::thread::id ThreadPool::PushTaskImpl(const TaskPtr& task, const std::thread::id& threadId,
                                         long long delay, ThreadType type /*= TT_UNKNOWN*/) {
    LOG_ENTER;
    if (nullptr == task) {
        return ThreadPool::InvalidThreadId();
    }

    std::unique_lock<std::mutex> lock(m_mutex);
    auto itor = m_listThreadPoolInner.find(type);
    if (m_listThreadPoolInner.end() == itor) {
        LOGE("not find {}", type);
        return ThreadPool::InvalidThreadId();
    }

    std::thread::id runThreadId = itor->second->PushTask(task, delay, threadId);
    LOG_LIVE;
    return runThreadId;
}

void ThreadPool::Clear(void) {
    LOG_ENTER;
    std::unique_lock<std::mutex> lock(m_mutex);
    for (auto& item : m_listThreadPoolInner) {
        item.second->Stop();
    }

    m_listThreadPoolInner.clear();
    LOG_LIVE;
}
}
