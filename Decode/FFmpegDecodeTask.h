#ifndef MP3DFILEECODE_H
#define MP3DFILEECODE_H

#include <string>

#include "Core/Constant.h"
#include "Core/MediaHandleTask.h"
#include "Core/FFMpegContext.h"


namespace XSPlayer {

    class FFmpegDecodeTask : public MediaHandleTask {
        using supper = MediaHandleTask;
    public:
        FFmpegDecodeTask(const MediaHandleChainPtr& pHandleChain, const FFMpegContextPtr& pDecodeContext);
        ~FFmpegDecodeTask(void) override;

    protected:
        virtual void DoTask(void);

    private:
        bool DecodePacket(AVCodecContext* pAVCodecContext, const AVPacket* packet);

    private:
        FFMpegContextPtr m_pDecodeContext;
    };

    using FFmpegDecodeTaskPtr = std::shared_ptr<FFmpegDecodeTask>;
}

#endif // MP3DFILEECODE_H
