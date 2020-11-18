#include "AudioRenderChain.h"

#include "Core/Log.h"
#include "Request/PlayCachRequest.h"
#include "Request/PlaySeekRequest.h"
#include "Request/PlayControlRequest.h"
#include "Core/FFMpegContextRequest.h"
#include "Core/MediaManager.h"
#include "Render/RenderEvent.h"

namespace XSPlayer {

    AudioRenderChain::AudioRenderChain(const MediaHandleChainPtr& pHandleChain, ThreadType threadType) : supper(pHandleChain, threadType){
    }

    AudioRenderChain::~AudioRenderChain(void) {
    }


    bool AudioRenderChain::Init(const FFMpegContextPtr& pContext) {
        m_pAudioRenderSDL = std::make_shared<AudioRenderSDL>(this);
        m_pAudioRenderSDL->Init(pContext);
        return true;
    }

    bool AudioRenderChain::HandleNext(const FFMpegContextWPtr& pWContext, const FFMpegMediaFramePtr& pMediaFrame) {
        FFMpegContextPtr pContext = pWContext.lock();
        if (nullptr == pContext) {
            LOGE("ffmpegcontext is release");
            return false;
        }

        m_pAudioRenderSDL->Render(pWContext, pMediaFrame);
        return true;
    }


    bool AudioRenderChain::HandleRequest(const RequestID rid, ChainRequest* request) {
        if (RequestID::RID_SEEK_PLAY == rid) {
            return SeekPlay(dynamic_cast<PlaySeekRequest*>(request));
        }
        else if (RequestID::RID_FFMPEG_CONTEXT == rid) {
            return InitFFMpeg(dynamic_cast<FFMpegContextRequest*>(request));
        }
        else if (RequestID::RID_PLAY_CONTROL == rid) {
            return PlayControl(dynamic_cast<PlayControlRequest*>(request));
        }
        return false;
    }

    bool AudioRenderChain::HandleEOF(void) {
        if (nullptr == m_pAudioRenderSDL) {
            return false;
        }

        m_pAudioRenderSDL->Uninit();
        m_pAudioRenderSDL.reset();
        return true;
    }


    bool AudioRenderChain::Stop(void) {
        if (nullptr == m_pAudioRenderSDL) {
            return true;
        }
        m_pAudioRenderSDL->Uninit();
        m_pAudioRenderSDL.reset();
        return true;
    }

    bool AudioRenderChain::Pause(void) {
        if (nullptr == m_pAudioRenderSDL) {
            return false;
        }

        m_pAudioRenderSDL->Pause();
        return true;
    }

    bool AudioRenderChain::Continue(void) {
        if (nullptr == m_pAudioRenderSDL) {
            return false;
        }

        m_pAudioRenderSDL->Continue();
        return true;
    }

    void AudioRenderChain::UpdateRenderListSize(size_t len) {
        if (len > 10) {
            PlayCacheRequest request(PlayCacheRequest::Common::CtrlCacheFull, true);
            LastHandleRequest(&request);
        }
        else if (len < 5) {
            PlayCacheRequest request(PlayCacheRequest::Common::CtrlCacheEmpty, true);
            LastHandleRequest(&request);
        }
    }


    void AudioRenderChain::UpdateRenderPostion(float len) {
        EventPtr pEvnet = RenderEvent::Create(RenderEvent::Type::RENDER_POS, len);
        MediaManager::GetSingleton().NotifyEvent(pEvnet);
    }

    bool AudioRenderChain::SeekPlay(PlaySeekRequest* request) {
        if (nullptr == request || nullptr == m_pAudioRenderSDL) {
            return false;
        }

        m_pAudioRenderSDL->PlaySeek();
        return true;
    }

    bool AudioRenderChain::InitFFMpeg(FFMpegContextRequest* request) {
        if (nullptr == request) {
            return false;
        }

        return Init(request->GetFFMpegContext());
    }

    bool AudioRenderChain::PlayControl(PlayControlRequest* request) {
        if (nullptr == request) {
            return false;
        }

        PlayControlRequest::Common common = request->GetCommon();
        switch (common) {
        case  PlayControlRequest::Common::CtrlPause:
            Pause();
            break;

        case PlayControlRequest::Common::CtrlContinue:
            Continue();
            break;

        case PlayControlRequest::Common::CtrlStop:
            Stop();
            break;

        default:
            break;
        }
        return true;
    }

}
