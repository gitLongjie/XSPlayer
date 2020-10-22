#ifndef MEDIAHANDLECHAIN_H
#define MEDIAHANDLECHAIN_H

#include <memory>
#include <string>

#include "Core/Constant.h"
#include "Core/FFMpegContext.h"
#include "Core/ChainRequest.h"
#include "Contanst.h"

namespace XSPlayer {

    class MediaHandleChain;
    using MediaHandleChainPtr = std::shared_ptr<MediaHandleChain>;
    using MediaHandleChainWPtr = std::weak_ptr<MediaHandleChain>;

    class MediaHandleTask;

    class MediaHandleChain {
        NON_COPY_ABLE(MediaHandleChain)

    public:
        MediaHandleChain(const MediaHandleChainPtr& pNextChain, ThreadType threadType);
        virtual ~MediaHandleChain(void) ;

        MediaHandleChainPtr GetNextChain(void) const { return m_pNextChain; }
        void SetLastChain(const MediaHandleChainPtr& pLastChain);
        virtual bool HandleEOF(void);
        virtual bool Stop(void);
        virtual bool Pause(void);
        virtual bool Continue(void);
        virtual bool NextHandleRequest(ChainRequest* reqeuest);
        virtual bool LastHandleRequest(ChainRequest* request);
        virtual bool HandleNext(const FFMpegContextWPtr& pWContext, const FFMpegMediaFramePtr& pMediaFrame);
        virtual bool HandleRequest(const RequestID rid, ChainRequest* reqeuest);

    protected:
        ThreadType GetThreadType(void) const { return m_hendleThreadType; }

        std::thread::id m_runThreadId;
    private:
        ThreadType m_hendleThreadType = ThreadType::TT_UNKNOWN;
        MediaHandleChainPtr m_pNextChain;
        MediaHandleChainWPtr m_pLastChain;
    };
}

#endif // MEDIAFRAME_H
