#include "Request/PlayCachRequest.h"

#include "Core/Application.h"
#include "Core/Log.h"
#include "IO/IOHandleChain.h"

namespace XSPlayer {

    PlayCacheRequest::PlayCacheRequest(PlayCacheRequest::Common ctrl, bool bBroadcast)
        :supper(bBroadcast), m_ctrl(ctrl){
    }


    PlayCacheRequest::~PlayCacheRequest(void) {

    }

    bool PlayCacheRequest::Vistor(MediaHandleChain* pHandleChain, bool& bNextChain)  {
        __super::Vistor(pHandleChain, bNextChain);

        return pHandleChain->HandleRequest(RequestID::RID_CACHE_CONTROL, this);
    }


    void PlayCacheRequest::CacheEmpty() {
        m_ctrl = Common::CtrlCacheEmpty;
    }

    void PlayCacheRequest::CacheFull() {
        m_ctrl = Common::CtrlCacheFull;
    }

}
