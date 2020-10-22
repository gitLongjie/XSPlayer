#include "DecodeHandleChain.h"

#include "Core/Application.h"
#include "Core/Log.h"
#include "Core/FFMpegContext.h"
#include "Decode/FFmpegDecodeTask.h"

namespace XSPlayer {

    DecodeHandleChain::DecodeHandleChain(const MediaHandleChainPtr& pHandleChain, ThreadType threadType)
    :supper(pHandleChain, threadType){
    }

    DecodeHandleChain::~DecodeHandleChain(void) {
    }

    bool DecodeHandleChain::HandleNext(const FFMpegContextWPtr& pWContext, const FFMpegMediaFramePtr& pMediaFrame) {
        FFMpegContextPtr pContext = pWContext.lock();
        if (nullptr == pContext) {
            return false;
        }

        if (pContext->IsPause()) { return false; }
        Decode(pContext, pMediaFrame);
        return true;
    }


    bool DecodeHandleChain::Stop(void) {
        return true;
    }

    void DecodeHandleChain::Decode(const FFMpegContextPtr& pContext, const FFMpegMediaFramePtr& pMediaFrame) {
        if (!pContext->Decodec(pMediaFrame, [this, pContext](const FFMpegMediaFramePtr& pMediaFrame) {
            supper::HandleNext(pContext, pMediaFrame);
        })) {
            LOGE("decode failed");
            return ;
        }

        pMediaFrame->DeletePacket();
        
    }
}
