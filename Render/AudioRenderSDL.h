#ifndef AUDIORENDERTASK_H
#define AUDIORENDERTASK_H

#include <memory>
#include <condition_variable>
#include <SDL2/SDL.h>
#include <queue>
#include <mutex>

#include "Core/MediaHandleTask.h"
#include "Core/TaskQueue.h"
#include "Core/FFMpegContext.h"

namespace XSPlayer {
    class AudioRenderChain;

    struct SDLRenderMediaFrame {
        SDLRenderMediaFrame(const FFMpegContextWPtr& pWContext, const FFMpegMediaFramePtr pMediaFrame)
            :m_pWContext(pWContext), m_pMediaFrame(pMediaFrame) {
        }
        FFMpegContextWPtr m_pWContext;
        FFMpegMediaFramePtr m_pMediaFrame;
    };
    using SDLRenderMediaFramePtr = std::shared_ptr<SDLRenderMediaFrame>;

    class AudioRenderSDL {
    public:
        AudioRenderSDL(AudioRenderChain* pAudioChain);
        ~AudioRenderSDL(void);


        void Init(const FFMpegContextPtr& pContext);
        void Uninit(void);
        int Render(const FFMpegContextWPtr& pWContext, const FFMpegMediaFramePtr pMediaFrame);

        void Pause() const;
        void Continue() const;
        void PlaySeek(void);

    private:
        static void FillAuido(void* udata, Uint8* stream, int len);
        bool OpenAudio(const FFMpegContextPtr& pContext);
        void RenderAuido(Uint8* stream, int len);
        bool Resample();
        SDLRenderMediaFramePtr GetRenderMediaFrame(void);
        void Clear(void);


    private:
        SDL_AudioDeviceID device;
        bool m_isOpenAudio = false;

        std::mutex m_mutex;
        std::condition_variable m_conditionVariable;
        using ListSDLRenderMediaFrame = std::queue<SDLRenderMediaFramePtr>;
        ListSDLRenderMediaFrame m_listRenderMediaFrame;
        std::atomic_bool m_bPlaying = false;
        uint8_t* m_audioBuffer = nullptr;
        unsigned int m_audioBufferSize = 0;
        int m_audioBufferIndex = 0;
        AudioRenderChain* m_pAudioRenderChain;
    };

    using AudioRenderSDLPtr = std::shared_ptr<AudioRenderSDL>;
}

#endif // AUDIORENDERTASK_H
