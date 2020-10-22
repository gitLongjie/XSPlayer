#pragma once

#include <memory>
#include <functional>

#include "Core/Constant.h"

struct AVFormatContext;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
struct AVPacket;
struct SwrContext;
struct AVRational;

namespace XSPlayer {

    class FFMpegMediaFrame {
        NON_COPY_ABLE(FFMpegMediaFrame)

    public:
        FFMpegMediaFrame();
        ~FFMpegMediaFrame();

        AVPacket* NewPacket(void);
        void DeletePacket(void);
        AVFrame* NewFrame(void);
        void DeleteFrame(void);
        void Clearup(void);

        AVPacket* GetPacket(void) const { return m_pAVPacket; }
        AVFrame* GetFrame(void) const { return m_pAVFrame; }

    private:
        AVPacket* m_pAVPacket = nullptr;
        AVFrame* m_pAVFrame = nullptr;
    };
    using FFMpegMediaFramePtr = std::shared_ptr<FFMpegMediaFrame>;

    struct OutAudioParam {
        int format;
        unsigned long long channelLayout;
        int freq;
        int nbSample;
        int channels;
        int frameSize;
        int bytesPerSec;
    };

    using DecoderFrame = std::function<void(FFMpegMediaFramePtr)>;
    class FFMpegContext {
        NON_COPY_ABLE(FFMpegContext)

    public:
        FFMpegContext(const String& mediaFilePath);
        ~FFMpegContext();

        bool Init(void);
        bool IsInited(void) const { return m_bInited; }
        AVFormatContext* GetFormatContext(void) const { return m_pFormatContext; }
        AVCodecContext* GetCodecContext(void) const { return m_pAVCodeContext; }
        SwrContext* GetSwrContext(void) const { return m_pSwrContenxt; }
        AVFrame* GetFrame(void) const { return m_pAVFrame; }
        AVPacket* GetPacket(void) const { return m_pAVPacket; }

        long long GetChannelLayout(void) const;
        int GetChannels(void) const;
        int GetSampleRate(void) const;
        
        static long long GetDefaultChannelLayout(int channels);
        static int GetChannelsByChannelLayout(long long channelLayout);
        static int GetBufferSizeBySample(int channels, int nbSample, int format, bool align);
        static int GetBufferSizeBySample(const FFMpegMediaFramePtr& pFFMpegeMediaFrame);

        int Resmple(const FFMpegMediaFramePtr& pFFMpegeMediaFrame, unsigned char** pAudioData, int& pts);

        FFMpegMediaFramePtr ReadPack(bool& isEof);
        bool Decodec(const FFMpegMediaFramePtr& pMediaFrame, DecoderFrame decodeFrame);
        unsigned int GetDuration(void) { return m_duration; }


        void Pause(bool pause);
        bool IsPause(void) const { return m_bPause; }

        OutAudioParam& GetOutAudioParam(void) { return m_outAudioParam; }
        const OutAudioParam& GetOutAudioParam(void) const { return m_outAudioParam; }
        void UpdataAudioParam() { m_outAudioParamSrc = m_outAudioParam; }

        bool SeekCur(long long seek);

    private:
        bool BuildFromFile(void);
        bool InitSwrContext(void);
        bool InitDecodeParam(void);
        void Cleanup(void);

    private:
        String m_mediaFilePath;
        bool m_bInited = false;
        AVFormatContext* m_pFormatContext = nullptr;
        AVCodecContext* m_pAVCodeContext = nullptr;
        SwrContext* m_pSwrContenxt = nullptr;
        AVFrame* m_pAVFrame = nullptr;
        AVPacket* m_pAVPacket = nullptr;
        bool m_bPause = false;
        unsigned int m_bufferSize = 0;
        unsigned int m_duration = 0;
        long long m_nextPts = 0;
        AVRational* m_nextPtsTb;

        OutAudioParam m_outAudioParam;
        OutAudioParam m_outAudioParamSrc;
    };

    using FFMpegContextPtr = std::shared_ptr<FFMpegContext>;
    using FFMpegContextWPtr = std::weak_ptr<FFMpegContext>;
}

