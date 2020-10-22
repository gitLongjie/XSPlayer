#ifndef APPHANDLETASKQUEUE_H
#define APPHANDLETASKQUEUE_H

#pragma once

#include <memory>

#include "Core/ThreadPoolInner.h"
#include "Core/Constant.h"
#include "Core/TaskQueue.h"

namespace XSPlayer {
    class AppHandleTaskQueue : public IThreadPoolInner {
        NON_COPY_ABLE(AppHandleTaskQueue)
    public:
        AppHandleTaskQueue();
        ~AppHandleTaskQueue(void) override;

        std::thread::id PushTask(const TaskPtr& task, long long delay, const std::thread::id& threadId)  override;
        void Stop()  override;
    //    bool IsRunning() const override;
        ThreadType GetType(void)  override;
        size_t GetIdleCount(void) const override;

        void OnRun();

    private:
        TaskQueuePtr m_pTaskQueue;
    };

    using AppHandleTaskQueuePtr = std::shared_ptr<AppHandleTaskQueue>;
}
#endif  // APPHANDLETASKQUEUE_H

