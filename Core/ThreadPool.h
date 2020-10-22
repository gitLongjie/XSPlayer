#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Constant.h"
#include "Singleton.h"
#include "Task.h"
#include "Core/ThreadPoolInner.h"

namespace XSPlayer {

class ThreadPool : public Singleton<ThreadPool> {
    NON_COPY_ABLE(ThreadPool)

 public:
  ThreadPool();
  ~ThreadPool(void);

  void RegistThreadPoolInner(const IThreadPoolInnerPtr& threadPoolInner);
  void UnregistTaskQueue(const IThreadPoolInnerPtr& threadPoolInner);
  void UnregistTaskQueue(ThreadType type);

  static const std::thread::id& InvalidThreadId(void);
  static bool IsInvalideThreadId(const std::thread::id& threadId);
  static  std::thread::id PushTask(const TaskPtr& task, const std::thread::id& threadId, long long delay = 0,
                                   ThreadType type = ThreadType::TT_UNKNOWN);
  std::thread::id PushTaskImpl(const TaskPtr& task, const std::thread::id& threadId, long long delay = 0,
                               ThreadType type = ThreadType::TT_UNKNOWN);
  void Clear(void);

 private:
     std::mutex m_mutex;
     std::unordered_map<ThreadType, IThreadPoolInnerPtr> m_listThreadPoolInner;
};

using ThreadPoolPtr = std::shared_ptr<ThreadPool>;
}  // namespace XSPlayer
#endif  // THREADPOOL_H
