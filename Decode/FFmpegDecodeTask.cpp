#include "FFmpegDecodeTask.h"

#include "Contanst.h"

#include "Core/Log.h"
#include "Core/MediaFrame.h"

#define AUDIO_INBUF_SIZE 20480

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

namespace XSPlayer {

    FFmpegDecodeTask::FFmpegDecodeTask(const MediaHandleChainPtr& pHandleChain, 
                                       const FFMpegContextPtr& pDecodeContext)
        : supper()
        , m_pDecodeContext(pDecodeContext){

    }

    FFmpegDecodeTask::~FFmpegDecodeTask(void) {
    }

    void FFmpegDecodeTask::DoTask(void) {
        //m_pMediaFrame.reset();
        
        if (nullptr == m_pDecodeContext || !m_pDecodeContext->IsInited()) {
            LOGE("decodecontext is nullptr or is init failed, {}", long long(m_pDecodeContext.get()));
            return;
        }

        AVFormatContext* pAVForamtContext = m_pDecodeContext->GetFormatContext();
        AVCodecContext* pAVCodecContext = m_pDecodeContext->GetCodecContext();
        AVPacket* pAVPacket = m_pDecodeContext->GetPacket();

        if (av_read_frame(pAVForamtContext, pAVPacket) < 0) {
            return;
        }

        DecodePacket(pAVCodecContext, pAVPacket);
        av_packet_unref(pAVPacket);        
    }


    bool FFmpegDecodeTask::DecodePacket(AVCodecContext* pAVCodecContext, const AVPacket* packet) {
        int ret = avcodec_send_packet(pAVCodecContext, packet);
        if (ret < 0) {
            LOGE("avcodec_send_packet failed");
            return false;
        }

        AVFrame* pAVFrame = m_pDecodeContext->GetFrame();
        SwrContext* pSwrContext = m_pDecodeContext->GetSwrContext();

        //AVFrame* frame = nullptr;
        while (ret >= 0) {
            ret = avcodec_receive_frame(pAVCodecContext, pAVFrame);
            if (ret < 0) {
                if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                    break;

                LOGE("avcodec_receive_frame failed");
                return false;
            }
       //     int len = av_samples_get_buffer_size(nullptr, pAVCodecContext->channels, pAVFrame->nb_samples, pAVCodecContext->f, 1);
            
//            int bufferSize = av_samples_get_buffer_size(NULL, pAVCodecContext->channels, pAVCodecContext->frame_size, AV_SAMPLE_FMT_S16, 1);

//             m_pMediaFrame = std::make_shared<MediaFrame>();
//             m_pMediaFrame->Alloc(bufferSize);
//             m_pMediaFrame->SetOutSampleRate(pAVCodecContext->sample_rate);
//             m_pMediaFrame->SetChannel(pAVCodecContext->channels);
//             m_pMediaFrame->SetSample(pAVCodecContext->frame_size);
// 
//             uint8_t* outBuffer = m_pMediaFrame->Data();
//             swr_convert(pSwrContext,&outBuffer, bufferSize, (const uint8_t**)pAVFrame->data, pAVFrame->nb_samples);
//            
//             supper::HandleMediaFrame(m_pMediaFrame);
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
            av_frame_unref(pAVFrame);
        }
        return true;
    }


}
