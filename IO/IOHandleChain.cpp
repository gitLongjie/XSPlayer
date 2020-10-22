#include "IO/IOHandleChain.h"

#include <functional>

#include "Core/FFMpegContext.h"
#include "Core/Log.h"
#include "Core/ThreadPool.h"
#include "Core/FFMpegContextRequest.h"
#include "Request/PlaySeekRequest.h"
#include "Core/MediaManager.h"

namespace XSPlayer {

    IOHandleChain::IOHandleChain(const MediaHandleChainPtr& pHandleChain, ThreadType threadType)
        :supper(pHandleChain, threadType) {
    }

    IOHandleChain::~IOHandleChain(void) {
    }

    bool IOHandleChain::OpenStream(const String& mediaPath) {
        FFMpegContextPtr pContext = std::make_shared<FFMpegContext>(mediaPath);

        TaskPtr pTask = CreateTask(std::bind(&IOHandleChain::InitContext, this, pContext));
        m_runThreadId = ThreadPool::PushTask(pTask, m_runThreadId, 0, GetThreadType());
        return ThreadPool::IsInvalideThreadId(m_runThreadId);
    }


    bool IOHandleChain::Init(FFMpegContextRequest* request) {
        if (nullptr == request) {
            return false;
        }

        FFMpegContextPtr pContext = request->GetFFMpegContext();
        TaskPtr pTask = CreateTask(std::bind(&IOHandleChain::OnInit, this, pContext));
        ThreadPool::PushTask(pTask, m_runThreadId, 0, GetThreadType());
        return true;
    }

    bool IOHandleChain::HandleRequest(const RequestID rid, ChainRequest* request) {
        if (RequestID::RID_SEEK_PLAY == rid) {
            return Jump(dynamic_cast<PlaySeekRequest*>(request));
        }
        else if (RequestID::RID_FFMPEG_CONTEXT == rid) {
            return false; // Init(dynamic_cast<FFMpegContextRequest*>(request));
        }
        else if (RequestID::RID_CACHE_CONTROL == rid) {
            return CacheControl(dynamic_cast<PlayCacheRequest*>(request));
        }
        else if (RequestID::RID_PLAY_CONTROL == rid) {
            return PlayControl(dynamic_cast<PlayControlRequest*>(request));
        }
        return false;
    }

    bool IOHandleChain::Stop(void) {
        m_pFFMpegContext.reset();
        return true;
    }

    bool IOHandleChain::Pause() {
        m_bPauseRead = true;
        return true;
    }

    bool IOHandleChain::Continue(void) {
        if (m_bEof || !m_bPauseRead) {
            return false;
        }
        m_bPauseRead = false;
        TaskPtr pTask = CreateTask(std::bind(&IOHandleChain::ReadFrame, this, m_pFFMpegContext));
        ThreadPool::PushTask(pTask, m_runThreadId, 0, GetThreadType());
        return true;
    }


    bool IOHandleChain::Jump(PlaySeekRequest* request) {
        if (nullptr == request) {
            return false;
        }

        long long seek = request->GetSeekPosition();
        TaskPtr pTask = CreateTask(std::bind(&IOHandleChain::OnJump, this, seek));
        return ThreadPool::PushTask(pTask, m_runThreadId, 0, GetThreadType()) == m_runThreadId;
    }

    bool IOHandleChain::HandleMeidaFile(const String& mediaFile) {
        return false;
    }


    void IOHandleChain::InitContext(const FFMpegContextPtr& pContext) {
        if (nullptr == pContext) {
            return;
        }

        if (!pContext->Init()) {
            LOGE("ffmpegcontext init failed");
            return;
        }

        FFMpegContextRequest request(pContext, true);
        NextHandleRequest(&request);

        MediaInfo info = { 1, pContext->GetDuration() };
        MediaManager::GetSingleton().UpdateMediaInfo(info);
        m_bPauseRead = false;
        m_bEof = false;

        OnInit(pContext);
        TaskPtr pTask = CreateTask(std::bind(&IOHandleChain::ReadFrame, this, pContext));
        ThreadPool::PushTask(pTask, m_runThreadId, 0, GetThreadType());
    }

    void IOHandleChain::ReadFrame(const FFMpegContextWPtr& pContext) {
        if (m_bPauseRead) {
            return;
        }

        FFMpegContextPtr pFFmpegContext = pContext.lock();
        if (nullptr == pFFmpegContext) {
            LOGI("FFMpegContext is released");
            return;
        }

        bool bEof = false;
        FFMpegMediaFramePtr pMediaFrame = pFFmpegContext->ReadPack(bEof);
        if (nullptr == pMediaFrame) {
            if (bEof) {
                PlayControlRequest playControl;
                playControl.PlayEOF();
                //HandleRequest(playControl);
                LOGI("read eof");
                m_bEof = bEof;
                return;
            }
            LOGI("read packet is nullptr");
            return;
        }

        if (!HandleNext(pContext, pMediaFrame)) {
            LOGE("handle next failed");
            return;
        }

        TaskPtr pTask = CreateTask(std::bind(&IOHandleChain::ReadFrame, this, pContext));
        ThreadPool::PushTask(pTask, m_runThreadId, 20, GetThreadType());
        //std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }


    void IOHandleChain::OnInit(const FFMpegContextPtr& pContext) {
        m_pFFMpegContext = pContext;
    }

    void IOHandleChain::OnJump(long long seek) {
        if (nullptr == m_pFFMpegContext) {
            return;
        }

        unsigned int duration = m_pFFMpegContext->GetDuration();
        if (seek > duration) {
            m_bPauseRead = false;
            m_bEof = false;
            return;
        }

       m_pFFMpegContext->SeekCur(seek);
    }


    void IOHandleChain::OnCacheControl(PlayCacheRequest::Common common) {
        switch (common) {
        case PlayCacheRequest::Common::CtrlCacheEmpty:
            Continue();
            break;
        case PlayCacheRequest::Common::CtrlCacheFull:
            Pause();
            break;
        default:
            break;
        }
    }


    void IOHandleChain::OnPlayControl(PlayControlRequest::Common common) {
        switch (common) {
        case  PlayControlRequest::Common::CtrlUnkown:
            break;

        case  PlayControlRequest::Common::CtrlPause:
            Pause();
            break;

        case PlayControlRequest::Common::CtrlContinue:
            Continue();
            break;

        case PlayControlRequest::Common::CtrlEOF:

        case PlayControlRequest::Common::CtrlStop:
            Stop();
            break;

        default:
            break;
        } 
    }

    bool IOHandleChain::CacheControl(PlayCacheRequest* request) {
        if (nullptr == request) {
            return false;
        }

        PlayCacheRequest::Common common = request->GetCommon();
        TaskPtr pTask = CreateTask(std::bind(&IOHandleChain::OnCacheControl, this, common));
        ThreadPool::PushTask(pTask, m_runThreadId, 0, GetThreadType());
        return true;
    }

    bool IOHandleChain::PlayControl(PlayControlRequest* request) {
        if (nullptr == request) {
            return false;
        }

        auto common = request->GetCommon();
        if (PlayControlRequest::Common::CtrlPlay == common) {
            const String& mediaPath = request->GetString();
            OpenStream(mediaPath);
            return true;
        }

        TaskPtr pTask = CreateTask(std::bind(&IOHandleChain::OnPlayControl, this, common));
        ThreadPool::PushTask(pTask, m_runThreadId, 0, GetThreadType());
        return true;
    }

}
