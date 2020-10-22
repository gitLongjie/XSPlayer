#pragma once

#include "Core/Constant.h"

namespace XSPlayer {
    class MediaHandleChain;
    class ChainRequest {
        NON_COPY_ABLE(ChainRequest)

    public:
        explicit ChainRequest(bool bBroadcast);
        virtual ~ChainRequest();

        virtual bool Vistor(MediaHandleChain* pHandleChain, bool& bNextChain);

    protected:
        bool IsBroadcast(void) const;

    private:
        bool m_bBroadcast = false;
    };

}

