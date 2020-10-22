#ifndef IOHANDLECHAIN_H
#define IOHANDLECHAIN_H

#include <memory>
#include "Core/Constant.h"
#include "Core/MediaHandleChain.h"
#include "Core/FFMpegContext.h"
#include "Request/PlayCachRequest.h"
#include "Request/PlayControlRequest.h"

namespace XSPlayer {

    class FFMpegContextRequest;
    class PlaySeekRequest;

    class IOHandleChain : public MediaHandleChain {
        using supper = MediaHandleChain;
    public:
        IOHandleChain(const MediaHandleChainPtr& pHandleChain, ThreadType threadType);
        ~IOHandleChain(void) override;

        // bool HandleMediaFrame(const MediaFramePtr& pMediaFrame) override;
        bool OpenStream(const String& mediaPath);
        bool HandleRequest(const RequestID rid, ChainRequest* reqeuest);
        bool Stop(void) override;
        bool Pause(void) override;
        bool Continue(void) override;

        bool Jump(PlaySeekRequest* request);
        bool Init(FFMpegContextRequest* request);
        bool CacheControl(PlayCacheRequest* request);
        bool PlayControl(PlayControlRequest* request);

    protected:
        bool HandleMeidaFile(const String& mediaFile);

    private:
        void InitContext(const FFMpegContextPtr& pContext);
        void ReadFrame(const FFMpegContextWPtr& pContext);
        void OnInit(const FFMpegContextPtr& pContext);
        void OnJump(long long step);
        void OnCacheControl(PlayCacheRequest::Common common);
        void OnPlayControl(PlayControlRequest::Common common);

    private:
        FFMpegContextPtr m_pFFMpegContext;
        bool m_bPauseRead = false;
        bool m_bEof = false;
    };
    using IOHandleChainPtr = std::shared_ptr<IOHandleChain>;
}

#endif // IOHANDLECHAIN_H
