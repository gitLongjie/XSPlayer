#include "Render/AudioRenderSDL.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include "libavformat/avformat.h"
#ifdef __cplusplus
}
#endif

#include "Core/Log.h"
#include "Core/FFMpegContext.h"
#include "Render/AudioRenderChain.h"

namespace XSPlayer {
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

    AudioRenderSDL::AudioRenderSDL(AudioRenderChain* pAudioChain) : m_pAudioRenderChain(pAudioChain) {

    }

    AudioRenderSDL::~AudioRenderSDL(void) {

    }

    void AudioRenderSDL::Init(const FFMpegContextPtr& pContext) {
        SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

        if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
            LOGE("Couldn't initialize SDL: {}\n", SDL_GetError());
            return;
        }

        OpenAudio(pContext);
        m_bPlaying = true;
    }

    void AudioRenderSDL::Uninit(void) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (!m_isOpenAudio) {
                return;
            }
            m_isOpenAudio = false;
            m_bPlaying = false;
            Clear();
            m_conditionVariable.notify_one();
        }
        
        SDL_CloseAudioDevice(device);
        SDL_Quit();
    }

    int AudioRenderSDL::Render(const FFMpegContextWPtr& pWContext, const FFMpegMediaFramePtr pMediaFrame) {
        if (!m_isOpenAudio || !m_bPlaying) {
            return false;
        }

        SDLRenderMediaFramePtr pRenderMediaFrame = std::make_shared<SDLRenderMediaFrame>(pWContext,
                                                                                         pMediaFrame);
        std::unique_lock<std::mutex> lock(m_mutex);
        m_listRenderMediaFrame.push(pRenderMediaFrame);
        m_conditionVariable.notify_one();
        return m_listRenderMediaFrame.size();
    }


    void AudioRenderSDL::Pause() const {
        SDL_PauseAudioDevice(device, 1);
    }

    void AudioRenderSDL::Continue() const {
        SDL_PauseAudioDevice(device, 0);
    }

    void AudioRenderSDL::PlaySeek(void) {
        std::unique_lock<std::mutex> lock(m_mutex);
        Clear();
    }

    void AudioRenderSDL::FillAuido(void* udata, Uint8* stream, int len) {
        AudioRenderSDL* pRender = static_cast<AudioRenderSDL*>(udata);
        if (nullptr == pRender) {
            return;
        }

        pRender->RenderAuido(stream, len);
    }


    bool AudioRenderSDL::OpenAudio(const FFMpegContextPtr& pContext) {
        long long channelLayout = pContext->GetChannelLayout();
        int channels = pContext->GetChannels();
        int sampleRate = pContext->GetSampleRate();


        const char* evn = SDL_getenv("SDL_AUDIO_CHANNELS");
        if (nullptr != evn) {
            channels = atoi(evn);
            channelLayout = pContext->GetDefaultChannelLayout(channels);
        }

        if (0 == channelLayout || channels != pContext->GetChannelsByChannelLayout(channelLayout)) {
            channelLayout = pContext->GetDefaultChannelLayout(channels);
            channelLayout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
        }
        channels = pContext->GetChannelsByChannelLayout(channelLayout);

        static const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };
        static const int next_sample_rates[] = { 0, 44100, 48000, 96000, 192000 };
        int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

        SDL_AudioSpec wanted_spec, spec;
        wanted_spec.freq = sampleRate;
        wanted_spec.channels = channels;
        if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
            // av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channel count!\n");
            LOGE("invalide sample rate or channel count");
            return false;
        }
        while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq) {
            next_sample_rate_idx--;
        }

        wanted_spec.format = AUDIO_S16SYS;
        wanted_spec.silence = 0;
        wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE,
                                    2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
        wanted_spec.callback = &AudioRenderSDL::FillAuido;
        wanted_spec.userdata = this;

        while (!(device = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE))) {
            av_log(NULL, AV_LOG_WARNING, "SDL_OpenAudio (%d channels, %d Hz): %s\n",
                   wanted_spec.channels, wanted_spec.freq, SDL_GetError());
            wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
            if (!wanted_spec.channels) {
                wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
                wanted_spec.channels = channels;
                if (!wanted_spec.freq) {
                    LOGE("No more combinations to try, audio open failed");
                    return false;
                }
            }
            channelLayout = pContext->GetDefaultChannelLayout(wanted_spec.channels);
        }

        if (spec.format != AUDIO_S16SYS ) {
            LOGE("sdl not support foamt or channel is error");
            return false;
        }

        if (spec.channels != wanted_spec.channels) {
            channelLayout = pContext->GetDefaultChannelLayout(spec.channels);
            if (!channelLayout) {
                LOGE("sdl advised channel count {} is not supported");
                return false;
            }
        }

        OutAudioParam& outAudioParam = pContext->GetOutAudioParam();
        outAudioParam.format = AV_SAMPLE_FMT_S16;
        outAudioParam.channelLayout = channelLayout;
        outAudioParam.channels = spec.channels;
        outAudioParam.freq = spec.freq;
        outAudioParam.frameSize = pContext->GetBufferSizeBySample(spec.channels, 1, outAudioParam.format, 1);
        outAudioParam.bytesPerSec = pContext->GetBufferSizeBySample(spec.channels, outAudioParam.freq, outAudioParam.format, 1);
        if (outAudioParam.bytesPerSec <= 0 || outAudioParam.frameSize <= 0) {
            LOGE("GetBufferSizeSample failed");
            return false;
        }
        pContext->UpdataAudioParam();

        m_isOpenAudio = true;

        SDL_PauseAudioDevice(device, 0);
        return true;
    }


    void AudioRenderSDL::RenderAuido(Uint8* stream, int len) {
        int len1 = 0;

        while (len > 0 && m_bPlaying)
        {
            if (m_audioBufferIndex >= static_cast<int>(m_audioBufferSize)) {
                if (!Resample()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    continue;
                }

                m_audioBufferIndex = 0;
            }

            len1 = m_audioBufferSize - m_audioBufferIndex;
            if (len1 > len) {
                len1 = len;
            }
            memcpy(stream, (uint8_t*)m_audioBuffer + m_audioBufferIndex, len1);
            m_audioBufferIndex += len1;
            len -= len1;
            stream += len1;
        }
 
    }

#define SAMPLE_ARRAY_SIZE (8 * 65536)
    bool AudioRenderSDL::Resample() {
        SDLRenderMediaFramePtr pRenderMediaFrame = GetRenderMediaFrame();
        if (nullptr == pRenderMediaFrame) {
            return false;
        }

        FFMpegContextPtr pContext = pRenderMediaFrame->m_pWContext.lock();
        if (nullptr == pContext) {
            return false;
        }

        FFMpegMediaFramePtr pMediaFrame = pRenderMediaFrame->m_pMediaFrame;
        if (nullptr == pMediaFrame) {
            return false;
        }

        int pts = 0;
        int dataSize = pContext->Resmple(pMediaFrame, &m_audioBuffer, pts);
        if (dataSize < 0) {
            return false;
        }

        m_audioBufferSize = dataSize;

        m_pAudioRenderChain->UpdateRenderPostion(pts);
        return true;
    }

    SDLRenderMediaFramePtr AudioRenderSDL::GetRenderMediaFrame(void) {
        while (m_listRenderMediaFrame.empty()) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_conditionVariable.wait(lock, [this]() {return !m_bPlaying.load() || !m_listRenderMediaFrame.empty(); });
            if (!m_bPlaying.load()) {
                return nullptr;
            }
        }

        if (!m_bPlaying) {
            return nullptr;
        }

        SDLRenderMediaFramePtr pRenderMediaFrame = m_listRenderMediaFrame.front();
        m_listRenderMediaFrame.pop();

        m_pAudioRenderChain->UpdateRenderListSize(m_listRenderMediaFrame.size());
        return pRenderMediaFrame;
    }

    void AudioRenderSDL::Clear(void) {
        while (!m_listRenderMediaFrame.empty()) {
            m_listRenderMediaFrame.pop();
        }

        FFMpegContextPtr emptyContext = std::make_shared<FFMpegContext>(_T(""));
        SDLRenderMediaFramePtr pRenderMediaFrame = std::make_shared<SDLRenderMediaFrame>(emptyContext,
                                                                                         nullptr);
        m_listRenderMediaFrame.push(pRenderMediaFrame);
    }

}
