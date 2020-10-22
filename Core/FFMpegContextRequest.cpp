#include "Core/FFMpegContextRequest.h"

#include "Core/MediaHandleChain.h"

namespace XSPlayer {
    
    FFMpegContextRequest::FFMpegContextRequest(const FFMpegContextPtr& pContext, bool bBroadcast)
        : supper(bBroadcast)
        , m_pFFMpegContext(pContext) {

    }

    FFMpegContextRequest::~FFMpegContextRequest() {

    }

    bool FFMpegContextRequest::Vistor(MediaHandleChain* pHandleChain, bool& bNextChain) {
        __super::Vistor(pHandleChain, bNextChain);
        if (nullptr == pHandleChain || nullptr == m_pFFMpegContext) {
            bNextChain = false;
            return true;
        }

        return pHandleChain->HandleRequest(RequestID::RID_FFMPEG_CONTEXT, this);
    }

    void FFMpegContextRequest::ContextInitFinish() {

    }

}