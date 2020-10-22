#ifndef PLAYSEEKREQUEST_H
#define PLAYSEEKREQUEST_H

#include <memory>
#include "Core/Constant.h"
#include "Core/ChainRequest.h"

namespace XSPlayer {

    class PlaySeekRequest : public ChainRequest {
        using supper = ChainRequest;
    public:
        explicit PlaySeekRequest(long long pos, bool bBroadcast);
        ~PlaySeekRequest(void) override;

        bool Vistor(MediaHandleChain* pHandleChain, bool& bNextChain) override;

    public:
        long long GetSeekPosition(void) const;

    private:
        long long m_seekPos = 0;
    };
    using PlaySeekRequestPtr = std::shared_ptr<PlaySeekRequest>;
}

#endif // PLAYSEEKREQUEST_H
