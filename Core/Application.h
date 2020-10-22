#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <string>
#include <functional>
#include "Singleton.h"
#include "ThreadPool.h"
#include "Core/AppHandleTaskQueue.h"
#include "Core/MediaManager.h"

namespace XSPlayer {
class Application : public Singleton<Application> {
  public:
    Application(void);
    virtual ~Application(void) override;

    void OnRunTask(void);
    inline ThreadPoolPtr GetThreadPool(void) const { return m_pThreadPool; }

    virtual void Init(void) = 0;

  private:
    ThreadPoolPtr m_pThreadPool;
    AppHandleTaskQueuePtr m_pAppHandleTaskQueue;
    MediaManagerPtr m_pMediaManager;
};
}

#endif // APPLICATION_H
