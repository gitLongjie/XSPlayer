#pragma once

#include "Core/Constant.h"
#include "Core/ChainRequest.h"
#include "Core/FFMpegContext.h"

namespace XSPlayer {
    class MediaHandleChain;

    class FFMpegContextRequest : public ChainRequest{
        using supper = ChainRequest;
        NON_COPY_ABLE(FFMpegContextRequest);

    public:
        explicit FFMpegContextRequest(const FFMpegContextPtr& pContext, bool bBroadcast);
        ~FFMpegContextRequest() override;

        bool Vistor(MediaHandleChain* pHandleChain, bool& bNextChain) override;

        void ContextInitFinish();
        FFMpegContextPtr GetFFMpegContext(void) const { return m_pFFMpegContext; }

    private:
        FFMpegContextPtr m_pFFMpegContext;
    };
}


