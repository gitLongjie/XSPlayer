#include "Core/FFMpegContext.h"

#include <locale>
#include <codecvt>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#ifdef __cplusplus
}
#endif

#include "Core/Constant.h"
#include "Core/Log.h"
#include "Request/PlayControlRequest.h"
#include "Utils.h"

const double TIME_BASE = 1.0 / AV_TIME_BASE;

namespace XSPlayer {

    FFMpegContext::FFMpegContext(const String& mediaFilePath) : m_mediaFilePath(mediaFilePath) {

    }

    FFMpegContext::~FFMpegContext() {
        Cleanup();
    }

    bool FFMpegContext::Init(void) {
        LOG_ENTER;
        if (m_mediaFilePath.empty()) {
            LOGE("media file path is empty");
            return false;
        }

        m_bInited = BuildFromFile();

        m_nextPtsTb = new AVRational();// { 1, 1 };
        LOG_LIVE;
        return true;
    }

    long long FFMpegContext::GetChannelLayout(void) const {
        if (nullptr == m_pAVCodeContext) {
            return -1;
        }

        return m_pAVCodeContext->channel_layout;
    }

    int FFMpegContext::GetChannels(void) const {
        if (nullptr == m_pAVCodeContext) {
            return -1;
        }

        return m_pAVCodeContext->channels;
    }

    int FFMpegContext::GetSampleRate(void) const {
        if (nullptr == m_pAVCodeContext) {
            return -1;
        }

        return m_pAVCodeContext->sample_rate;
    }

    long long FFMpegContext::GetDefaultChannelLayout(int channels) {
        return av_get_default_channel_layout(channels);
    }

    int FFMpegContext::GetChannelsByChannelLayout(long long channelLayout) {
        return av_get_channel_layout_nb_channels(channelLayout);
    }

    int FFMpegContext::GetBufferSizeBySample(int channels, int nbSample, int format, bool align) {
        return av_samples_get_buffer_size(nullptr, channels, nbSample, (AVSampleFormat)format, align);
    }

    int FFMpegContext::GetBufferSizeBySample(const FFMpegMediaFramePtr& pFFMpegeMediaFrame) {
        if (nullptr == pFFMpegeMediaFrame) {
            return -1;
        }

        AVFrame* pAVFrame = pFFMpegeMediaFrame->GetFrame();
        if (nullptr == pAVFrame) {
            return -1;
        }

        return GetBufferSizeBySample(pAVFrame->channels, pAVFrame->nb_samples, pAVFrame->format, 1);
    }

    int FFMpegContext::Resmple(const FFMpegMediaFramePtr& pFFMpegeMediaFrame, unsigned char** pAudioData, float& pts) {
        if (nullptr == pFFMpegeMediaFrame) {
            return -1;
        }

        AVFrame* pFrame = pFFMpegeMediaFrame->GetFrame();
        if (nullptr == pFrame) {
            return -2;;
        }

        unsigned long long channelLayout = pFrame->channel_layout && pFrame->channels == GetChannelsByChannelLayout(pFrame->channel_layout) ?
            pFrame->channel_layout : GetDefaultChannelLayout(pFrame->channels);
 
         if (pFrame->format != m_outAudioParamSrc.format ||
             channelLayout != m_outAudioParamSrc.channelLayout ||
             pFrame->sample_rate != m_outAudioParamSrc.freq ||
             nullptr == m_pSwrContenxt) {
             swr_free(&m_pSwrContenxt);
 
             m_pSwrContenxt = swr_alloc_set_opts(nullptr,
                                                 m_outAudioParam.channelLayout, (AVSampleFormat)m_outAudioParam.format,
                                                 m_outAudioParam.freq,
                                                 channelLayout, (AVSampleFormat)pFrame->format, pFrame->sample_rate,
                                                 0, nullptr);
             if (nullptr == m_pSwrContenxt) {
                 LOGE("swr_alloc_get_opts failed");
                 return -1;
             }
 
             if (swr_init(m_pSwrContenxt) < 0) {
                 LOGE("swr_init failed");
                 swr_free(&m_pSwrContenxt);
                 return -1;
             }
 
             m_outAudioParamSrc.format = pFrame->format;;
             m_outAudioParamSrc.channelLayout = channelLayout;
             m_outAudioParamSrc.freq = pFrame->sample_rate;
             m_outAudioParamSrc.channels = pFrame->channels;
         }
 
         if (nullptr != m_pSwrContenxt) {
             int out_count = (int64_t)pFrame->nb_samples * m_outAudioParam.freq / pFrame->sample_rate + 256;
             int outSize = av_samples_get_buffer_size(nullptr, m_outAudioParam.channels, out_count,
                                                      (AVSampleFormat)m_outAudioParam.format, 0);
             if (outSize < 0) {
                 LOGE("av_sample_get_bufer_size failed");
                 return -1;
             }
 
 //            *pAudioData = new unsigned char[outSize] {0};
 //             av_samples_alloc(pOutFrame->data, pOutFrame->linesize, m_outAudioParam.channels, m_outAudioParam.nbSample,
 //                              (AVSampleFormat)m_outAudioParam.format, 0);
             av_fast_malloc(pAudioData, &m_bufferSize, outSize);
             int len = swr_convert(m_pSwrContenxt, pAudioData, out_count, (const uint8_t**)pFrame->extended_data, pFrame->nb_samples);
             if (len < 0) {
              //   delete[] * pAudioData;
                 LOGE("swr_convert failed");
                 return -1;
             }
 
             if (len == outSize) {
                 if (swr_init(m_pSwrContenxt) < 0) {
                     swr_free(&m_pSwrContenxt);
                     m_pSwrContenxt = nullptr;
                 }
             }

             AVRational tb = { 1, pFrame->sample_rate };
             pts = static_cast<float>(pFrame->pts * av_q2d(tb) + (double)pFrame->nb_samples / pFrame->sample_rate);
             return len * m_outAudioParam.channels * av_get_bytes_per_sample((AVSampleFormat)m_outAudioParam.format);
             return outSize; // ret * 3 * av_get_bytes_per_sample((AVSampleFormat)m_outAudioParam.format);
         }
         else {
             int outSize = av_samples_get_buffer_size(nullptr, m_outAudioParam.channels, m_outAudioParam.nbSample,
                                                      (AVSampleFormat)m_outAudioParam.format, 1);
             int dataLen = outSize > pFrame->linesize[0] ? pFrame->linesize[0] : outSize;
             *pAudioData = new unsigned char[outSize] {0};
             memcpy(pAudioData, pFrame->data[0],dataLen);
             pts = static_cast<float>(pFrame->pts * TIME_BASE);
             return dataLen;
         }
    }

    FFMpegMediaFramePtr FFMpegContext::ReadPack(bool& isEof) {
        isEof = false;

        if (!m_bInited) {
            return nullptr;
        }

        FFMpegMediaFramePtr pMediaFrame = std::make_shared<FFMpegMediaFrame>();
        AVPacket* packet = pMediaFrame->NewPacket();
        if (nullptr == packet) {
            return nullptr;
        }

        int ret = av_read_frame(m_pFormatContext, packet);
        if (ret < 0) {
            if ((AVERROR_EOF == ret || avio_feof(m_pFormatContext->pb))) {
                isEof = true;
            }
            return nullptr;
        }

        return std::move(pMediaFrame);
    }

    bool FFMpegContext::Decodec(const FFMpegMediaFramePtr& pMediaFrame, DecoderFrame decodeFrame) {
        if (nullptr == pMediaFrame || nullptr == m_pAVCodeContext || nullptr == decodeFrame) {
            LOGE("mediaframe is nullptr {} or vacode ccntext is nullptr {}",
                 long long(pMediaFrame.get()), long long(m_pAVCodeContext));
            return false;
        }

        AVPacket* pAVPacket = pMediaFrame->GetPacket();
        if (nullptr == pAVPacket) {
            LOGE("avpacket is nullptr");
            return false;
        }

        int ret = avcodec_send_packet(m_pAVCodeContext, pAVPacket);
        if (ret < 0) {
            LOGE("avcodec_send_packet failed");
            return false;
        }

        while (ret >= 0) {
            FFMpegMediaFramePtr pFrameMediaFrame = std::make_shared<FFMpegMediaFrame>();
            AVFrame* pAVFrame = pFrameMediaFrame->NewFrame();
            if (nullptr == pAVFrame) {
                LOGE("avframe is nullptr");
                return false;
            }
            ret = avcodec_receive_frame(m_pAVCodeContext, pAVFrame);
            if (ret < 0) {
                if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                    break;

                LOGE("avcodec_receive_frame failed");
                return false;
            }
            
            AVRational tb = { 1, pAVFrame->sample_rate };
            if (pAVFrame->pts != AV_NOPTS_VALUE) {
                pAVFrame->pts = av_rescale_q(pAVFrame->pts, m_pAVCodeContext->pkt_timebase, tb);
            }
            else if (m_nextPts != AV_NOPTS_VALUE) {
                pAVFrame->pts = av_rescale_q(m_nextPts, *m_nextPtsTb, tb);
            }

            if (pAVFrame->pts != AV_NOPTS_VALUE) {
                m_nextPts = pAVFrame->pts + pAVFrame->nb_samples;
                *m_nextPtsTb = tb;
            }
            
            decodeFrame(pFrameMediaFrame);
        }

        return true;
    }

    void FFMpegContext::Pause(bool pause) {
        m_bPause = pause;
    }

    bool FFMpegContext::SeekCur(long long seek) {
        if (nullptr == m_pAVCodeContext) {
            return false;
        }

        if (seek >= m_duration) {
            return false;
        }

//         int64_t seek_min = is->seek_rel > 0 ? seek_target - is->seek_rel + 2 : INT64_MIN;
//         int64_t seek_max = is->seek_rel < 0 ? seek_target - is->seek_rel - 2 : INT64_MAX;
        avformat_seek_file(m_pFormatContext, -1, 0, seek * AV_TIME_BASE, m_duration * AV_TIME_BASE, 0);
        return true;
    }

    std::string wstring_to_utf8(const std::wstring& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.to_bytes(str);
    }

    bool FFMpegContext::BuildFromFile(void) {
        LOG_ENTER;
#if defined(_UNICODE) || defined(UNICODE)
        std::string filePath = wstring_to_utf8(m_mediaFilePath);
#else
        std::string filePath = m_mediaFilePath;
#endif 

        int ret = avformat_open_input(&m_pFormatContext, filePath.c_str(), nullptr, nullptr);
        if (ret < 0) {
            LOGE("aformat_open_input failed");
            return false;
        }

        ret = avformat_find_stream_info(m_pFormatContext, nullptr);
        if (ret < 0) {
            LOGE("avformat_find_stream_info failed");
            return false;
        }

        int streamId = av_find_best_stream(m_pFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
        if (streamId < 0) {
            LOGE("av_find_best_stream failed");;
            return false;
        }

        m_duration = static_cast<int>(m_pFormatContext->duration * TIME_BASE);

        AVDictionaryEntry* tag = nullptr;
        while (tag = av_dict_get(m_pFormatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)) {
#ifdef _UNICODE
            String keyString = Utils::StringToWString(tag->key);
            String valueString =  Utils::StringToWString(tag->value);
#else
            String keyString = tag->key;
            String valueString = tag->value;
#endif
          //  m_InfoMap.insert(keyString, valueString);
        }

        AVStream* pAVStream = m_pFormatContext->streams[streamId];
        if (nullptr == pAVStream) {
            LOGE("get stream failed");
            return false;
        }

        AVCodec* pDecode = avcodec_find_decoder(pAVStream->codecpar->codec_id);
        if (nullptr == pDecode) {
            LOGE("avcode_find_decoder failed");
            return false;
        }

        m_pAVCodeContext = avcodec_alloc_context3(pDecode);
        if (nullptr == m_pAVCodeContext) {
            LOGE("avcodec_alloc_context3 failed");
            return false;
        }

        ret = avcodec_parameters_to_context(m_pAVCodeContext, pAVStream->codecpar);
        if (ret < 0) {
            LOGE("avcodec_parameters_to_context failed");
            return false;
        }

        m_pAVCodeContext->pkt_timebase = m_pFormatContext->streams[0]->time_base;
        ret = avcodec_open2(m_pAVCodeContext, pDecode, nullptr);
        if (ret < 0) {
            LOGE("avcode_open2 failed");
            return false;
        }

//         if (!InitSwrContext()) {
//             LOGE("init swrContext failed");
//             return false;
//         }

//         if (!InitDecodeParam()) {
//             LOGE("InitDecodeParam failed");
//             return false;
//         }

        LOG_LIVE;
        return true;
    }

    bool FFMpegContext::InitSwrContext(void) {
        LOG_ENTER;

        m_pSwrContenxt = swr_alloc_set_opts(NULL, (int64_t)AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, m_pAVCodeContext->sample_rate,
                                            av_get_default_channel_layout(m_pAVCodeContext->channels), m_pAVCodeContext->sample_fmt,
                                            m_pAVCodeContext->sample_rate, 0, NULL);
        // m_pSwrContenxt = swr_alloc();
        if (nullptr == m_pSwrContenxt) {
            LOGE("swr_alloc failed");
            return false;
        }

        int ret = swr_init(m_pSwrContenxt);
        if (ret < 0) {
            LOGE("swr_init failed");
            swr_free(&m_pSwrContenxt);
            m_pSwrContenxt = nullptr;
            return false;
        }

        LOG_LIVE;
        return true;
    }

    bool FFMpegContext::InitDecodeParam(void) {
        LOG_ENTER;

        m_pAVFrame = av_frame_alloc();
        if (nullptr == m_pAVFrame) {
            LOGE("av_frame_alloc failed");
            return false;
        }

        m_pAVPacket = av_packet_alloc();
        if (nullptr == m_pAVPacket) {
            LOGE("av_packet_alloc failed");
            return false;
        }

        av_init_packet(m_pAVPacket);
        m_pAVPacket->data = nullptr;
        m_pAVPacket->size = 0;

        LOG_LIVE;

        return true;
    }

    void FFMpegContext::Cleanup(void) {
        LOG_ENTER;

        if (nullptr != m_pAVPacket) {
            av_packet_unref(m_pAVPacket);
            av_packet_free(&m_pAVPacket);
            m_pAVPacket = nullptr;
        }

        if (nullptr != m_pAVFrame) {
            av_frame_unref(m_pAVFrame);
            av_frame_free(&m_pAVFrame);
            m_pAVFrame = nullptr;
        }

        if (nullptr != m_pSwrContenxt) {
            swr_free(&m_pSwrContenxt);
            m_pSwrContenxt = nullptr;
        }

        if (nullptr != m_pAVCodeContext) {
            avcodec_free_context(&m_pAVCodeContext);
            m_pAVCodeContext = nullptr;
        }

        if (nullptr != m_pFormatContext) {
            avformat_close_input(&m_pFormatContext);
        }

        LOG_LIVE;
    }

    FFMpegMediaFrame::FFMpegMediaFrame() {

    }

    FFMpegMediaFrame::~FFMpegMediaFrame() {
        Clearup();
    }

    AVPacket* FFMpegMediaFrame::NewPacket(void) {
        DeletePacket();
        m_pAVPacket = av_packet_alloc();
        if (nullptr == m_pAVPacket) {
            return nullptr;
        }

        av_init_packet(m_pAVPacket);
        return m_pAVPacket;
    }

    void FFMpegMediaFrame::DeletePacket(void) {
        if (nullptr != m_pAVPacket) {
            av_packet_unref(m_pAVPacket);
            av_packet_free(&m_pAVPacket);
        }
        
    }

    AVFrame* FFMpegMediaFrame::NewFrame(void) {
        DeleteFrame();
        m_pAVFrame = av_frame_alloc();
        if (nullptr == m_pAVFrame) {
            return nullptr;
        }

        // av_init_(m_pAVFrame);
        return m_pAVFrame;
    }

    void FFMpegMediaFrame::DeleteFrame(void) {
        if (nullptr != m_pAVFrame) {
            av_frame_unref(m_pAVFrame);
            av_frame_free(&m_pAVFrame);
        }
    }

    void FFMpegMediaFrame::Clearup(void) {
        DeletePacket();
        DeleteFrame();
    }

}
