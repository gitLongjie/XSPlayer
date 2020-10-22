#ifndef AUDIORENDER_H
#define AUDIORENDER_H

#include <memory>
#include "Core/MediaHandleChain.h"
#include "Render/AudioRenderSDL.h"

namespace XSPlayer {
    class PlaySeekRequest;
    class FFMpegContextRequest;
    class PlayControlRequest;

    class AudioRenderChain : public MediaHandleChain {
        using supper = MediaHandleChain;
    public:
        AudioRenderChain(const MediaHandleChainPtr& pHandleChain, ThreadType threadType);
        ~AudioRenderChain(void) override;
        bool Init(const FFMpegContextPtr& pContext);
        bool HandleNext(const FFMpegContextWPtr& pWContext, const FFMpegMediaFramePtr& pMediaFrame) override;
        bool HandleRequest(const RequestID rid, ChainRequest* reqeuest) override;
        bool HandleEOF(void) override;
        bool Stop(void) override;
        bool Pause(void) override;
        bool Continue(void) override;

        void UpdateRenderListSize(size_t len);
        void UpdateRenderPostion(size_t len);

    public:
        bool SeekPlay(PlaySeekRequest* request);

    private:
        AudioRenderSDLPtr m_pAudioRenderSDL;
        bool InitFFMpeg(FFMpegContextRequest* request);
        bool PlayControl(PlayControlRequest* request);
    };

    using AudioRenderChainPtr = std::shared_ptr<AudioRenderChain>;
}

#endif // DECODE_H
