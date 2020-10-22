#ifndef PLAYCACACHREQUEST_H
#define PLAYCACACHREQUEST_H

#include <memory>
#include "Core/Constant.h"
#include "Core/ChainRequest.h"

namespace XSPlayer {

    class PlayCacheRequest : public ChainRequest {
        using supper = ChainRequest;
    public:
        enum class  Common {
            CtrlCacheEmpty,
            CtrlCacheFull
        };

    public:
        explicit PlayCacheRequest(PlayCacheRequest::Common ctrl, bool bBroadcast);
        ~PlayCacheRequest(void) override;

        bool Vistor(MediaHandleChain* pHandleChain, bool& bNextChain) override;
        void CacheEmpty();
        void CacheFull();

        Common GetCommon(void) const { return m_ctrl; }

    private:
        Common m_ctrl = Common::CtrlCacheEmpty;
    };
    using PlayRequestPtr = std::shared_ptr<PlayCacheRequest>;
}

#endif // PLAYCACACHREQUEST_H
