#include "DecodeManager.h"

namespace XSPlayer {
template<> DecodeManager* Singleton<DecodeManager>::m_pSingleton = nullptr;
DecodeManager::DecodeManager(void) {
}

 DecodeManager::~DecodeManager(void) {
     m_pThreadPool->UnregistTaskQueue(m_pAppHandleTaskQueue);
}
}
