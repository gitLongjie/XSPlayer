#ifndef DECODEMANAGER_H
#define DECODEMANAGER_H

#include <memory>
#include <string>
#include <functional>
#include "Core/Singleton.h"
#include "Core/ThreadPool.h"
#include "Core/AppHandleTaskQueue.h"

namespace XSPlayer {

class DecodeManager : public Singleton<DecodeManager> {
  public:
    DecodeManager(void);
    virtual ~DecodeManager(void) override;

  private:
    ThreadPoolPtr m_pThreadPool;
    AppHandleTaskQueuePtr m_pAppHandleTaskQueue;
};
}

#endif // DECODEMANAGER_H
