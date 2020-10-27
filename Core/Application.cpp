#include "Application.h"

namespace XSPlayer {
template<> Application* Singleton<Application>::m_pSingleton = nullptr;
Application::Application(void)
    : m_pThreadPool(new ThreadPool),
      m_pAppHandleTaskQueue(new AppHandleTaskQueue),
    m_pMediaManager(new MediaManager){
    m_pThreadPool->RegistThreadPoolInner(m_pAppHandleTaskQueue);
}

 Application::~Application(void) {
     m_pMediaManager->Destory();
     m_pThreadPool->UnregistTaskQueue(m_pAppHandleTaskQueue);
     m_pThreadPool.reset();
     m_pMediaManager.reset();
}

void Application::OnRunTask(void) { m_pAppHandleTaskQueue->OnRun(); }

}
