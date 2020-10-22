#ifndef DECODE_H
#define DECODE_H

#include <memory>
#include "Core/Constant.h"
#include "Core/MediaHandleChain.h"

namespace XSPlayer {

    class DecodeHandleChain : public MediaHandleChain, public std::enable_shared_from_this<DecodeHandleChain> {
        using supper = MediaHandleChain;
    public:
        DecodeHandleChain(const MediaHandleChainPtr& pHandleChain, ThreadType threadType);
        ~DecodeHandleChain(void) override;

    public:
        bool HandleNext(const FFMpegContextWPtr& pWContext, const FFMpegMediaFramePtr& pMediaFrame) override;
        bool Stop(void) override;

    protected:
        void Decode(const FFMpegContextPtr& pContext, const FFMpegMediaFramePtr& pMediaFrame);

    private:
        
        //void OnDecode();
    };
    using DecodeHandleChainPtr = std::shared_ptr<DecodeHandleChain>;
}

#endif // DECODE_H
