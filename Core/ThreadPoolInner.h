#ifndef THREADPOOLINNER_H
#define THREADPOOLINNER_H

#include "Constant.h"
#include "Task.h"

#include <thread>

namespace XSPlayer {

    class Thread;
class IThreadPoolInner{
 public:
     virtual ~IThreadPoolInner(void) = default;
     virtual std::thread::id PushTask(const TaskPtr& task, long long delay = 0,
                                      const std::thread::id& threadId = std::thread::id()) = 0;
     virtual void Stop() = 0;
/*     virtual bool IsRunning() const = 0;*/

     virtual ThreadType GetType(void) = 0;
     virtual size_t GetIdleCount(void) const = 0;
};

using IThreadPoolInnerPtr = std::shared_ptr<IThreadPoolInner>;
}  // namespace XSPlayer
#endif  // THREADPOOLINNER_H
