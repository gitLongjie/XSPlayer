#include "Core/MediaHandleChain.h"
#include "Core/ChainRequest.h"
#include "Core/Log.h"
#include "Core/ThreadPool.h"

namespace XSPlayer {

    MediaHandleChain::MediaHandleChain(const MediaHandleChainPtr& pNextChain, ThreadType threadType)
    : m_hendleThreadType(threadType),
    m_pNextChain(pNextChain) {

    }

    MediaHandleChain::~MediaHandleChain(void) {

    }

    void MediaHandleChain::SetLastChain(const MediaHandleChainPtr& pLastChain) {
        m_pLastChain = pLastChain;
    }


    bool MediaHandleChain::HandleEOF(void) {
        return false;
    }

    bool MediaHandleChain::Stop(void) {
        return false;
    }


    bool MediaHandleChain::Pause(void) {
        return false;
    }


    bool MediaHandleChain::Continue(void) {
        return false;
    }

    bool MediaHandleChain::NextHandleRequest(ChainRequest* request) {
        bool bNextChain = false;
        request->Vistor(this, bNextChain);

       // MediaHandleChainPtr pNextChain = m_pNextChain.lock();
        if (nullptr != m_pNextChain && bNextChain) {
            m_pNextChain->NextHandleRequest(request);
        }

        return bNextChain;
    }

    bool MediaHandleChain::LastHandleRequest(ChainRequest* request) {
        bool bLastChain = false;
        request->Vistor(this, bLastChain);

        MediaHandleChainPtr pLastChain = m_pLastChain.lock();
        if (nullptr != pLastChain && bLastChain) {
            pLastChain->LastHandleRequest(request);
        }

        return bLastChain;
    }

    bool MediaHandleChain::HandleNext(const FFMpegContextWPtr& pWContext, const FFMpegMediaFramePtr& pMediaFrame) {
        LOG_ENTER;

        //MediaHandleChainPtr pNextChain = m_pNextChain.lock();
        if (nullptr == m_pNextChain) {
            LOGE("next chain is nullptr");
            return false;
        }

        TaskPtr pTask = CreateTask(std::bind(&MediaHandleChain::HandleNext, m_pNextChain, pWContext, pMediaFrame));
        m_pNextChain->m_runThreadId = ThreadPool::PushTask(pTask, m_pNextChain->m_runThreadId,0, m_pNextChain->GetThreadType());
        return true;
    }


    bool MediaHandleChain::HandleRequest(const RequestID rid, ChainRequest* reqeuest) {
        return false;
    }

//     bool MediaHandleChain::HandleMediaFrame(const MediaFramePtr& pMediaFrame) {
//         if (nullptr == m_pNextChain) {
//             return false;
//         }
// 
//         return m_pNextChain->HandleMediaFrame(pMediaFrame);
//     }


}
