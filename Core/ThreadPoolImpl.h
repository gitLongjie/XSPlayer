#ifndef THREADPOOLIMPL_H
#define THREADPOOLIMPL_H

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Constant.h"
#include "Task.h"
#include "ThreadPoolInner.h"

namespace XSPlayer {

    class Thread;
    using ThreadPtr = std::shared_ptr<Thread>;

class ThreadPoolImpl : public IThreadPoolInner {
    NON_COPY_ABLE(ThreadPoolImpl)

 public:
  ~ThreadPoolImpl(void);

  std::thread::id PushTask(const TaskPtr& task, long long delay = 0,
                           const std::thread::id& threadId = std::thread::id()) override;
  void Stop(void) override;
 /* bool IsRunning() const override;*/

  ThreadType GetType(void) override { return m_type; }
  size_t GetIdleCount(void) const override;

 private:
  explicit ThreadPoolImpl(ThreadType type, size_t nThreadSize = std::thread::hardware_concurrency());

  std::mutex m_mutex;
  std::vector<ThreadPtr> m_threads;
  ThreadType m_type;

  friend class ThreadPoolImplFactory;
  friend class Thread;
};

using IOThreadPoolPtr = std::shared_ptr<ThreadPoolImpl>;
using UnkownThreadPoolPtr = std::shared_ptr<ThreadPoolImpl>;
using DecodecThreadPoolPtr = std::shared_ptr<ThreadPoolImpl>;

class ThreadPoolImplFactory {
public:
    static IOThreadPoolPtr CreateIOThreadPool();
    static UnkownThreadPoolPtr CreateUnkownPool();
    static DecodecThreadPoolPtr CreateDecodecPool();
};
}  // namespace XSPlayer
#endif  // THREADPOOLIMPL_H
